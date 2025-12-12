#include "sentry_chassis_controller/sentry_chassis_controller.h"
#include <pluginlib/class_list_macros.hpp>
#include <cmath>
#include <tf/transform_listener.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>

namespace sentry_chassis_controller {

void SentryChassisController::reconfigureCallback(sentry_chassis_controller::PIDConfig &config, uint32_t level) 
{
  std::lock_guard<std::mutex> lock(reconfigure_mutex_);
  
  if (level & 0x1) {
    pid_lf_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
    pid_rf_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
    pid_lb_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
    pid_rb_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
    ROS_INFO("Updated pivot PID: P=%.2f, I=%.2f, D=%.2f", config.pivot_p, config.pivot_i, config.pivot_d);
  }
  
  if (level & 0x2) {
    pid_lf_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
    pid_rf_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
    pid_lb_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
    pid_rb_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
    ROS_INFO("Updated wheel PID: P=%.2f, I=%.2f, D=%.2f", config.wheel_p, config.wheel_i, config.wheel_d);
  }
}

bool SentryChassisController::init(hardware_interface::EffortJointInterface *effort_joint_interface,ros::NodeHandle &root_nh, ros::NodeHandle &controller_nh) 
{
  // 获取关节句柄
  front_left_wheel_joint_ =
      effort_joint_interface->getHandle("left_front_wheel_joint");
  front_right_wheel_joint_ =
      effort_joint_interface->getHandle("right_front_wheel_joint");
  back_left_wheel_joint_ = 
      effort_joint_interface->getHandle("left_back_wheel_joint");
  back_right_wheel_joint_ =
      effort_joint_interface->getHandle("right_back_wheel_joint");

  front_left_pivot_joint_ =
      effort_joint_interface->getHandle("left_front_pivot_joint");
  front_right_pivot_joint_ =
      effort_joint_interface->getHandle("right_front_pivot_joint");
  back_left_pivot_joint_ = 
      effort_joint_interface->getHandle("left_back_pivot_joint");
  back_right_pivot_joint_ =
      effort_joint_interface->getHandle("right_back_pivot_joint");

  // 获取参数
  wheel_track_ = controller_nh.param("wheel_track", 0.362);
  wheel_base_ = controller_nh.param("wheel_base", 0.362);
  wheel_radius_ = controller_nh.param("wheel_radius", 0.076);
  cmd_vel_timeout_ = controller_nh.param("cmd_vel_timeout", 0.5);
  max_wheel_speed_ = controller_nh.param("max_wheel_speed", 10.0);
  debug_print_ = controller_nh.param("debug_print", false);
  publish_odom_ = controller_nh.param("publish_odom", true);
  publish_tf_ = controller_nh.param("publish_tf", true);
  odom_frame_id_ = controller_nh.param("odom_frame_id", std::string("odom"));
  base_frame_id_ = controller_nh.param("base_frame_id", std::string("base_link"));
  
  world_vel_mode_ = controller_nh.param("world_vel_mode", false);
  world_frame_id_ = controller_nh.param("world_frame_id", std::string("odom"));
  
  // 获取自锁参数
  enable_self_lock_ = controller_nh.param("enable_self_lock", true);
  self_lock_timeout_ = controller_nh.param("self_lock_timeout", 1.0);
  self_lock_angle_ = controller_nh.param("self_lock_angle", 0.785);
  in_self_lock_mode_ = false;
  
  // 获取加速度限制参数
  max_linear_acceleration_ = controller_nh.param("max_linear_acceleration", 2.0);
  max_angular_acceleration_ = controller_nh.param("max_angular_acceleration", 3.0);
  last_vx_ = last_vy_ = last_wz_ = 0.0;
  ax_actual_ = ay_actual_ = awz_actual_ = 0.0;
  
  //获取功率控制参数
  max_total_power_ = controller_nh.param("max_total_power", 200.0);
  power_limit_enabled_ = controller_nh.param("power_limit_enabled", true);
  current_power_limit_ = max_total_power_;
  power_filter_time_constant_ = controller_nh.param("power_filter_time_constant", 0.1);
  
  // 初始化功率相关变量
  total_power_ = 0.0;
  power_consumption_ = 0.0;
  power_regeneration_ = 0.0;
  power_limited_ = false;
  
  // 初始化轮子状态变量
  for (int i = 0; i < 4; ++i) {
    wheel_steering_angle_[i] = 0.0;
    wheel_speed_[i] = 0.0;
    wheel_torque_[i] = 0.0;
  }
  
  // 初始化PID控制器
  double pivot_p, pivot_i, pivot_d;
  controller_nh.param("pivot_p", pivot_p, 1.0);
  controller_nh.param("pivot_i", pivot_i, 0.0);
  controller_nh.param("pivot_d", pivot_d, 0.0);
  
  pid_lf_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_rf_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_lb_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_rb_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);

  double wheel_p, wheel_i, wheel_d;
  controller_nh.param("wheel_p", wheel_p, 2.0);
  controller_nh.param("wheel_i", wheel_i, 0.1);
  controller_nh.param("wheel_d", wheel_d, 0.0);
  
  pid_lf_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);
  pid_rf_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);
  pid_lb_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);
  pid_rb_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);

  // 初始化dynamic_reconfigure服务器
  dyn_reconfig_server_.reset(new dynamic_reconfigure::Server<sentry_chassis_controller::PIDConfig>(controller_nh));
  dyn_reconfig_callback_ = boost::bind(&SentryChassisController::reconfigureCallback, this, _1, _2);
  dyn_reconfig_server_->setCallback(dyn_reconfig_callback_);

  // 初始化速度指令
  vx_ = 0.0;
  vy_ = 0.0;
  wz_ = 0.0;
  last_cmd_vel_time_ = ros::Time::now();
  last_accel_time_ = ros::Time::now();
  last_power_time_ = ros::Time::now();

  // 初始化轮子命令
  for (int i = 0; i < 4; ++i) {
    pivot_cmd_[i] = 0.0;
    wheel_cmd_[i] = 0.0;
    last_valid_pivot_cmd_[i] = 0.0;
  }

  // 初始化里程计
  x_ = 0.0;
  y_ = 0.0;
  theta_ = 0.0;
  vx_actual_ = 0.0;
  vy_actual_ = 0.0;
  wz_actual_ = 0.0;

  // 初始化TF监听器
  if (world_vel_mode_) {
    tf_listener_.reset(new tf::TransformListener());
  }

  // 订阅cmd_vel话题
  ros::NodeHandle nh;
  cmd_vel_sub_ = nh.subscribe<geometry_msgs::Twist>("cmd_vel", 1, 
                    &SentryChassisController::cmdVelCallback, this);

  // 发布里程计话题
  if (publish_odom_) {
    odom_pub_ = nh.advertise<nav_msgs::Odometry>("odom", 50);
  }

  // 发布加速度话题
  accel_pub_ = nh.advertise<geometry_msgs::Accel>("chassis_acceleration", 50);
  
  //发布功率相关话题
  total_power_pub_ = nh.advertise<std_msgs::Float64>("chassis_power_total", 50);
  consumption_power_pub_ = nh.advertise<std_msgs::Float64>("chassis_power_consumption", 50);
  regeneration_power_pub_ = nh.advertise<std_msgs::Float64>("chassis_power_regeneration", 50);
  power_limit_pub_ = nh.advertise<std_msgs::Float64>("chassis_power_limit", 50);
  power_limited_pub_ = nh.advertise<std_msgs::Bool>("chassis_power_limited", 50);
  
  //发布轮子状态话题
  wheel_state_pub_ = nh.advertise<sentry_chassis_controller::WheelState>("wheel_states", 50);

  // 初始化TF广播器
  if (publish_tf_) {
    tf_broadcaster_.reset(new tf::TransformBroadcaster());
  }

  return true;
}

bool SentryChassisController::transformVelocityToBaseFrame(const geometry_msgs::Twist& world_vel, geometry_msgs::Twist& base_vel) {
  try {
    // 创建几何消息向量
    geometry_msgs::Vector3Stamped world_vel_vec;//将速度从世界坐标系转换到底盘坐标系
    geometry_msgs::Vector3Stamped base_vel_vec;
    
    world_vel_vec.header.stamp = ros::Time::now();
    world_vel_vec.header.frame_id = world_frame_id_;
    world_vel_vec.vector.x = world_vel.linear.x;
    world_vel_vec.vector.y = world_vel.linear.y;
    world_vel_vec.vector.z = world_vel.linear.z;
    
    // 使用 transformVector 进行坐标变换
    tf_listener_->transformVector(base_frame_id_, world_vel_vec, base_vel_vec);
    
    base_vel.linear.x = base_vel_vec.vector.x;
    base_vel.linear.y = base_vel_vec.vector.y;
    base_vel.linear.z = base_vel_vec.vector.z;
    base_vel.angular.z = world_vel.angular.z;  // 角速度保持不变
    
    return true;
  }
  catch (tf::TransformException &ex) {
    //ROS_WARN_THROTTLE(1.0, "TF transform failed: %s", ex.what());
    return false;
  }
}

void SentryChassisController::starting(const ros::Time& time) {
  // 重置PID控制器
  pid_lf_.reset();
  pid_rf_.reset();
  pid_lb_.reset();
  pid_rb_.reset();
  pid_lf_wheel_.reset();
  pid_rf_wheel_.reset();
  pid_lb_wheel_.reset();
  pid_rb_wheel_.reset();
  
  last_cmd_vel_time_ = time;
  last_odom_time_ = time;
  last_accel_time_ = time;
  last_power_time_ = time;
  in_self_lock_mode_ = false;
  last_vx_ = last_vy_ = last_wz_ = 0.0;
  ax_actual_ = ay_actual_ = awz_actual_ = 0.0;
  
  // 初始化功率相关变量
  total_power_ = 0.0;
  power_consumption_ = 0.0;
  power_regeneration_ = 0.0;
  power_limited_ = false;
  
  // 清空功率历史记录
  while (!power_history_.empty()) {
    power_history_.pop();
  }
  
  // 重置功率限制为最大值
  current_power_limit_ = max_total_power_;
  
  // 初始化轮子状态
  for (int i = 0; i < 4; ++i) {
    wheel_steering_angle_[i] = 0.0;
    wheel_speed_[i] = 0.0;
    wheel_torque_[i] = 0.0;
  }
}

void SentryChassisController::stopping(const ros::Time& time) 
{
  // 停止时先进入自锁模式
  if (enable_self_lock_) {
    enterSelfLockMode();
    // 等待一小段时间确保自锁完成
    ros::Duration(0.1).sleep();
  }
  
  // 停止时设置所有轮子速度为0
  front_left_wheel_joint_.setCommand(0.0);
  front_right_wheel_joint_.setCommand(0.0);
  back_left_wheel_joint_.setCommand(0.0);
  back_right_wheel_joint_.setCommand(0.0);
}

// 进入自锁模式
void SentryChassisController::enterSelfLockMode() {
  if (!enable_self_lock_ || in_self_lock_mode_) return;
  
  in_self_lock_mode_ = true;
  self_lock_start_time_ = ros::Time::now();
  
  if (debug_print_) {
    ROS_INFO("Entering self-lock mode");
  }
}

// 退出自锁模式
void SentryChassisController::exitSelfLockMode() {
  if (!in_self_lock_mode_) return;
  
  in_self_lock_mode_ = false;
  
  if (debug_print_) {
    ROS_INFO("Exiting self-lock mode");
  }
}

// 自锁状态下的轮子控制
void SentryChassisController::computeWheelCommandsForSelfLock() {
  // 计算每个轮子指向底盘中心的角度
  // 轮子位置（相对于底盘中心）
  double L = wheel_base_ / 2.0;
  double W = wheel_track_ / 2.0;
  
  pivot_cmd_[0] = atan2(-W, -L);
  
  pivot_cmd_[1] = atan2(W, -L);
  
  pivot_cmd_[2] = atan2(-W, L);
  
  pivot_cmd_[3] = atan2(W, L);
  
  for (int i = 0; i < 4; ++i) {
    // 使轮子稍微更向内扣，增加自锁效果
    pivot_cmd_[i] += self_lock_angle_;
    
    // 角度标准化到 [-π, π]
    while (pivot_cmd_[i] > M_PI) pivot_cmd_[i] -= 2.0 * M_PI;
    while (pivot_cmd_[i] < -M_PI) pivot_cmd_[i] += 2.0 * M_PI;
  }
  
  // 自锁状态下轮子速度为0
  for (int i = 0; i < 4; ++i) {
    wheel_cmd_[i] = 0.0;
  }
  
  if (debug_print_) {
    ROS_INFO_THROTTLE(2.0, "Self-lock mode: Wheels pointing inward at %.2f rad", self_lock_angle_);
  }
}

// 更新自锁状态
void SentryChassisController::updateSelfLockState(const ros::Time &time, const ros::Duration &period) {
  if (!enable_self_lock_) return;
  
  // 检查是否有速度指令
  bool cmd_vel_active = (time - last_cmd_vel_time_).toSec() < cmd_vel_timeout_;
  
  if (!cmd_vel_active) {
    // 没有速度指令，检查是否应该进入自锁模式
    double time_since_last_cmd = (time - last_cmd_vel_time_).toSec();
    
    if (time_since_last_cmd > self_lock_timeout_) {
      // 超过自锁超时时间，进入自锁模式
      if (!in_self_lock_mode_) {
        enterSelfLockMode();
      }
    }
  } else {
    // 有速度指令，如果处于自锁模式则退出
    if (in_self_lock_mode_) {
      exitSelfLockMode();
    }
  }
}

void SentryChassisController::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
  std::lock_guard<std::mutex> lock(cmd_vel_mutex_);
  
  // 检查速度是否非零
  bool has_velocity = fabs(msg->linear.x) > 0.01 || 
                      fabs(msg->linear.y) > 0.01 || 
                      fabs(msg->angular.z) > 0.01;
  
  if (has_velocity && in_self_lock_mode_) {
    // 如果收到速度指令且当前处于自锁模式，先退出自锁模式
    exitSelfLockMode();
  }
  
  // 应用加速度限制（对输入指令进行限幅）
  double target_vx = msg->linear.x;
  double target_vy = msg->linear.y;
  double target_wz = msg->angular.z;
  
  if (world_vel_mode_) {
    vx_ = target_vx;
    vy_ = target_vy;
    wz_ = target_wz;
  } else {
    // 在底盘坐标系下，应用加速度限制
    double dt = (ros::Time::now() - last_cmd_vel_time_).toSec();
    if (dt <= 0) dt = 0.01;  // 避免除零
    
    // 计算允许的最大速度变化
    double max_dv_linear = max_linear_acceleration_ * dt;
    double max_dv_angular = max_angular_acceleration_ * dt;
    
    // 对线速度进行加速度限制
    double dvx = target_vx - last_vx_;
    double dvy = target_vy - last_vy_;
    double dv = sqrt(dvx*dvx + dvy*dvy);
    
    if (dv > max_dv_linear)//超速处理 
    {
      double scale = max_dv_linear / dv;
      dvx *= scale;
      dvy *= scale;
      vx_ = last_vx_ + dvx;
      vy_ = last_vy_ + dvy;
    } else {
      vx_ = target_vx;
      vy_ = target_vy;
    }
    
    // 对角速度进行加速度限制
    double dwz = target_wz - last_wz_;
    if (fabs(dwz) > max_dv_angular) {
      dwz = (dwz > 0) ? max_dv_angular : -max_dv_angular;
      wz_ = last_wz_ + dwz;
    } else {
      wz_ = target_wz;
    }
    
    // 保存当前速度，用于下次加速度限制计算
    last_vx_ = vx_;
    last_vy_ = vy_;
    last_wz_ = wz_;
  }
  
  last_cmd_vel_time_ = ros::Time::now();
  
  if (debug_print_ && has_velocity) {
    if (world_vel_mode_) {
      ROS_INFO_THROTTLE(1.0, "Received world velocity command: vx=%.2f, vy=%.2f, wz=%.2f", 
                       vx_, vy_, wz_);
    } else {
      ROS_INFO_THROTTLE(1.0, "Received base velocity command: vx=%.2f, vy=%.2f, wz=%.2f (after accel limit)", 
                       vx_, vy_, wz_);
    }
  }
}

// 应用功率限制
void SentryChassisController::applyPowerLimits(double &torque_fl, double &torque_fr, double &torque_bl, double &torque_br,const double &vel_fl, const double &vel_fr,
        const double &vel_bl, const double &vel_br) 
{
  
  if (!power_limit_enabled_ || current_power_limit_ <= 0) {
    return;  // 功率限制未启用
  }
  
  // 计算每个电机的功率：P = τ * ω
  double power_fl = torque_fl * vel_fl;
  double power_fr = torque_fr * vel_fr;
  double power_bl = torque_bl * vel_bl;
  double power_br = torque_br * vel_br;
  
  // 计算总功率（正值表示消耗，负值表示再生）
  double total_consumption = 0.0;
  double total_regeneration = 0.0;
  
  if (power_fl > 0) total_consumption += power_fl;
  else total_regeneration -= power_fl;  // power_fl为负
  
  if (power_fr > 0) total_consumption += power_fr;
  else total_regeneration -= power_fr;
  
  if (power_bl > 0) total_consumption += power_bl;
  else total_regeneration -= power_bl;
  
  if (power_br > 0) total_consumption += power_br;
  else total_regeneration -= power_br;
  
  double net_power = total_consumption - total_regeneration;
  
  // 检查是否超过功率限制（只限制消耗功率）
  bool limited = false;
  if (total_consumption > current_power_limit_) {
    // 计算缩放因子
    double scale_factor = current_power_limit_ / total_consumption;
    
    // 等比例缩减所有正在消耗功率的电机的扭矩
    if (power_fl > 0) {
      torque_fl *= scale_factor;
      power_fl = torque_fl * vel_fl;
    }
    if (power_fr > 0) {
      torque_fr *= scale_factor;
      power_fr = torque_fr * vel_fr;
    }
    if (power_bl > 0) {
      torque_bl *= scale_factor;
      power_bl = torque_bl * vel_bl;
    }
    if (power_br > 0) {
      torque_br *= scale_factor;
      power_br = torque_br * vel_br;
    }
    
    // 重新计算功率
    total_consumption = 0.0;
    if (power_fl > 0) total_consumption += power_fl;
    if (power_fr > 0) total_consumption += power_fr;
    if (power_bl > 0) total_consumption += power_bl;
    if (power_br > 0) total_consumption += power_br;
    
    net_power = total_consumption - total_regeneration;
    limited = true;
    
    if (debug_print_) {
      ROS_WARN_THROTTLE(1.0, "Power limiting active: %.1fW > %.1fW, scaling by %.2f", 
                       total_consumption, current_power_limit_, scale_factor);
    }
  }
  
  // 更新功率统计
  {
    std::lock_guard<std::mutex> lock(power_mutex_);
    power_consumption_ = total_consumption;
    power_regeneration_ = total_regeneration;
    total_power_ = net_power;
    power_limited_ = limited;
    
    // 添加平滑滤波
    power_history_.push(net_power);
    if (power_history_.size() > POWER_HISTORY_SIZE) {
      power_history_.pop();
    }
  }
}

// 计算并发布功率信息
void SentryChassisController::computeAndPublishPower(const ros::Time &time, const ros::Duration &period) {
  // 发布总功率
  std_msgs::Float64 total_power_msg;
  total_power_msg.data = total_power_;
  total_power_pub_.publish(total_power_msg);
  
  // 发布消耗功率
  std_msgs::Float64 consumption_power_msg;
  consumption_power_msg.data = power_consumption_;
  consumption_power_pub_.publish(consumption_power_msg);
  
  // 发布再生功率
  std_msgs::Float64 regeneration_power_msg;
  regeneration_power_msg.data = power_regeneration_;
  regeneration_power_pub_.publish(regeneration_power_msg);
  
  // 发布当前功率限制
  std_msgs::Float64 power_limit_msg;
  power_limit_msg.data = current_power_limit_;
  power_limit_pub_.publish(power_limit_msg);
  
  // 发布功率限制状态
  std_msgs::Bool power_limited_msg;
  power_limited_msg.data = power_limited_;
  power_limited_pub_.publish(power_limited_msg);
  
  // 在debug模式下打印功率信息
  if (debug_print_) {
    ROS_INFO_THROTTLE(2.0, "Power: total=%.1fW, consumption=%.1fW, regeneration=%.1fW, limit=%.1fW, limited=%s", 
                     total_power_, power_consumption_, power_regeneration_, current_power_limit_,
                     power_limited_ ? "true" : "false");
  }
  
  last_power_time_ = time;
}

// 计算并发布轮子状态信息
void SentryChassisController::computeAndPublishWheelStates(const ros::Time &time, const ros::Duration &period) 
{
  // 获取轮子实际状态
  wheel_steering_angle_[0] = front_left_pivot_joint_.getPosition();
  wheel_steering_angle_[1] = front_right_pivot_joint_.getPosition();
  wheel_steering_angle_[2] = back_left_pivot_joint_.getPosition();
  wheel_steering_angle_[3] = back_right_pivot_joint_.getPosition();
  
  wheel_speed_[0] = front_left_wheel_joint_.getVelocity();
  wheel_speed_[1] = front_right_wheel_joint_.getVelocity();
  wheel_speed_[2] = back_left_wheel_joint_.getVelocity();
  wheel_speed_[3] = back_right_wheel_joint_.getVelocity();
  
  wheel_torque_[0] = front_left_wheel_joint_.getCommand();
  wheel_torque_[1] = front_right_wheel_joint_.getCommand();
  wheel_torque_[2] = back_left_wheel_joint_.getCommand();
  wheel_torque_[3] = back_right_wheel_joint_.getCommand();
  
  // 创建并发布轮子状态消息
  sentry_chassis_controller::WheelState wheel_state_msg;
  wheel_state_msg.header.stamp = time;
  wheel_state_msg.header.frame_id = base_frame_id_;
  
  // 填充转向角度
  for (int i = 0; i < 4; ++i) {
    wheel_state_msg.steering_angle[i] = wheel_steering_angle_[i];
    wheel_state_msg.wheel_speed[i] = wheel_speed_[i];
    wheel_state_msg.wheel_velocity[i] = wheel_speed_[i] * wheel_radius_;  // 转换为线速度
    wheel_state_msg.wheel_torque[i] = wheel_torque_[i];
    // 电流可以通过扭矩计算）
    wheel_state_msg.wheel_current[i] = wheel_torque_[i] * 0.5;  //简单估算，0.5是扭矩常数
  }
  
  wheel_state_pub_.publish(wheel_state_msg);
  
  // 在debug模式下打印轮子状态
  if (debug_print_) {
    ROS_INFO_THROTTLE(2.0, 
        "Wheel States - FL(angle: %.1f°, speed: %.2frad/s, torque: %.2fNm) "
        "FR(angle: %.1f°, speed: %.2frad/s, torque: %.2fNm)",
        wheel_steering_angle_[0] * 180.0/M_PI, wheel_speed_[0], wheel_torque_[0],
        wheel_steering_angle_[1] * 180.0/M_PI, wheel_speed_[1], wheel_torque_[1]);
    
    ROS_INFO_THROTTLE(2.0, 
        "Wheel States - BL(angle: %.1f°, speed: %.2frad/s, torque: %.2fNm) "
        "BR(angle: %.1f°, speed: %.2frad/s, torque: %.2fNm)",
        wheel_steering_angle_[2] * 180.0/M_PI, wheel_speed_[2], wheel_torque_[2],
        wheel_steering_angle_[3] * 180.0/M_PI, wheel_speed_[3], wheel_torque_[3]);
  }
}

void SentryChassisController::computeWheelCommands(double vx, double vy, double wz) {
  // 全向移动底盘逆运动学计算
  double L = wheel_base_ / 2.0;
  double W = wheel_track_ / 2.0;
  double R = wheel_radius_;
  
  // 计算每个轮子的速度向量
  double vx_fl = vx - wz * W;
  double vy_fl = vy + wz * L;
  
  double vx_fr = vx - wz * (-W);
  double vy_fr = vy + wz * L;
  
  double vx_bl = vx - wz * W;
  double vy_bl = vy + wz * (-L);
  
  double vx_br = vx - wz * (-W);
  double vy_br = vy + wz * (-L);
  
  double new_pivot_cmd[4];
  new_pivot_cmd[0] = atan2(vy_fl, vx_fl);
  new_pivot_cmd[1] = atan2(vy_fr, vx_fr);
  new_pivot_cmd[2] = atan2(vy_bl, vx_bl);
  new_pivot_cmd[3] = atan2(vy_br, vx_br);
  
  double speed_fl = sqrt(vx_fl * vx_fl + vy_fl * vy_fl);
  double speed_fr = sqrt(vx_fr * vx_fr + vy_fr * vy_fr);
  double speed_bl = sqrt(vx_bl * vx_bl + vy_bl * vy_bl);
  double speed_br = sqrt(vx_br * vx_br + vy_br * vy_br);
  
  wheel_cmd_[0] = speed_fl / R;
  wheel_cmd_[1] = speed_fr / R;
  wheel_cmd_[2] = speed_bl / R;
  wheel_cmd_[3] = speed_br / R;
  
  for (int i = 0; i < 4; ++i) {
    if (fabs(wheel_cmd_[i]) > max_wheel_speed_) {
      wheel_cmd_[i] = (wheel_cmd_[i] > 0) ? max_wheel_speed_ : -max_wheel_speed_;
    }
  }
  
  double total_speed = fabs(vx) + fabs(vy) + fabs(wz);
  if (total_speed < 0.01) {
    for (int i = 0; i < 4; ++i) {
      wheel_cmd_[i] = 0.0;
      // 使用上一次有效的转向角度
      pivot_cmd_[i] = last_valid_pivot_cmd_[i];
    }
  } else {
    for (int i = 0; i < 4; ++i) {
      pivot_cmd_[i] = new_pivot_cmd[i];
      last_valid_pivot_cmd_[i] = new_pivot_cmd[i];
    }
  }
  
  if (debug_print_) {
    ROS_INFO_THROTTLE(1.0, 
      "Wheel commands - FL(angle: %.2f, speed: %.2f) FR(angle: %.2f, speed: %.2f) "
      "BL(angle: %.2f, speed: %.2f) BR(angle: %.2f, speed: %.2f)", 
      pivot_cmd_[0], wheel_cmd_[0], pivot_cmd_[1], wheel_cmd_[1],
      pivot_cmd_[2], wheel_cmd_[2], pivot_cmd_[3], wheel_cmd_[3]);
  }
}

void SentryChassisController::computeOdometry(const ros::Time &time, const ros::Duration &period) //里程计实现
{
  // 获取轮子的实际角速度
  double wheel_vel[4] = {
    front_left_wheel_joint_.getVelocity(),
    front_right_wheel_joint_.getVelocity(),
    back_left_wheel_joint_.getVelocity(),
    back_right_wheel_joint_.getVelocity()
  };
  
  // 获取转向关节的实际角度
  double pivot_angle[4] = {
    front_left_pivot_joint_.getPosition(),
    front_right_pivot_joint_.getPosition(),
    back_left_pivot_joint_.getPosition(),
    back_right_pivot_joint_.getPosition()
  };
  
  double L = wheel_base_ / 2.0;
  double W = wheel_track_ / 2.0;
  double R = wheel_radius_;
  
  double wheel_linear_vel[4];
  for (int i = 0; i < 4; ++i) {
    wheel_linear_vel[i] = wheel_vel[i] * R;
  }
  
  double vx_wheel[4], vy_wheel[4];
  for (int i = 0; i < 4; ++i) {
    vx_wheel[i] = wheel_linear_vel[i] * cos(pivot_angle[i]);
    vy_wheel[i] = wheel_linear_vel[i] * sin(pivot_angle[i]);
  }
  

  //直接平均法计算底盘速度
  vx_actual_ = (vx_wheel[0] + vx_wheel[1] + vx_wheel[2] + vx_wheel[3]) / 4.0;
  vy_actual_ = (vy_wheel[0] + vy_wheel[1] + vy_wheel[2] + vy_wheel[3]) / 4.0;
  
  wz_actual_ = 0.0;
  double sum_wz = 0.0;
  int count = 0;
  
  for (int i = 0; i < 4; ++i) {
    double wheel_vx = vx_wheel[i];
    double wheel_vy = vy_wheel[i];
    
    double wheel_x, wheel_y;
    switch(i) {
      case 0:
        wheel_x = L; wheel_y = W;
        break;
      case 1:
        wheel_x = L; wheel_y = -W;
        break;
      case 2:
        wheel_x = -L; wheel_y = W;
        break;
      case 3:
        wheel_x = -L; wheel_y = -W;
        break;
    }
    
    double wz_contrib = ((wheel_vx - vx_actual_) * (-wheel_y) + (wheel_vy - vy_actual_) * wheel_x) / (wheel_x * wheel_x + wheel_y * wheel_y);
    
    if (!std::isnan(wz_contrib) && !std::isinf(wz_contrib)) {
      sum_wz += wz_contrib;
      count++;
    }
  }
  
  if (count > 0) {
    wz_actual_ = sum_wz / count;
  }
  
  double dt = period.toSec();
  if (dt > 0.0) {
    theta_ += wz_actual_ * dt;
    
    while (theta_ > M_PI) theta_ -= 2.0 * M_PI;
    while (theta_ < -M_PI) theta_ += 2.0 * M_PI;
    
    double world_vx = vx_actual_ * cos(theta_) - vy_actual_ * sin(theta_);
    double world_vy = vx_actual_ * sin(theta_) + vy_actual_ * cos(theta_);
    
    x_ += world_vx * dt;
    y_ += world_vy * dt;
  }
  
  if (publish_odom_ && odom_pub_.getNumSubscribers() > 0) {
    nav_msgs::Odometry odom;
    odom.header.stamp = time;
    odom.header.frame_id = odom_frame_id_;
    odom.child_frame_id = base_frame_id_;
    
    odom.pose.pose.position.x = x_;
    odom.pose.pose.position.y = y_;
    odom.pose.pose.position.z = 0.0;
    
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(theta_);
    odom.pose.pose.orientation = odom_quat;//获取四元数
    
    odom.twist.twist.linear.x = vx_actual_;
    odom.twist.twist.linear.y = vy_actual_;
    odom.twist.twist.angular.z = wz_actual_;
    
    odom_pub_.publish(odom);
  }
  
  if (publish_tf_ && tf_broadcaster_) {
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = time;
    odom_trans.header.frame_id = odom_frame_id_;
    odom_trans.child_frame_id = base_frame_id_;
    
    odom_trans.transform.translation.x = x_;
    odom_trans.transform.translation.y = y_;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(theta_);
    
    tf_broadcaster_->sendTransform(odom_trans);
  }
  
  last_odom_time_ = time;
}

// 计算并发布加速度
void SentryChassisController::computeAndPublishAcceleration(const ros::Time &time, const ros::Duration &period) {
  // 计算实际加速度（基于实际速度的变化率）
  double dt = period.toSec();
  if (dt > 0.0) {
    // 使用滤波平滑加速度计算
    double alpha = 0.1;  // 低通滤波系数
    
    // 计算瞬时加速度
    double ax_instant = (vx_actual_ - last_vx_) / dt;
    double ay_instant = (vy_actual_ - last_vy_) / dt;
    double awz_instant = (wz_actual_ - last_wz_) / dt;
    
    // 应用低通滤波
    ax_actual_ = ax_actual_ * (1 - alpha) + ax_instant * alpha;
    ay_actual_ = ay_actual_ * (1 - alpha) + ay_instant * alpha;
    awz_actual_ = awz_actual_ * (1 - alpha) + awz_instant * alpha;
    
    // 更新上一次的速度值
    last_vx_ = vx_actual_;
    last_vy_ = vy_actual_;
    last_wz_ = wz_actual_;
  }
  
  // 发布加速度消息
  geometry_msgs::Accel accel_msg;
  accel_msg.linear.x = ax_actual_;
  accel_msg.linear.y = ay_actual_;
  accel_msg.linear.z = 0.0;
  accel_msg.angular.x = 0.0;
  accel_msg.angular.y = 0.0;
  accel_msg.angular.z = awz_actual_;
  
  accel_pub_.publish(accel_msg);
  
  // 可选：在debug模式下打印加速度信息
  if (debug_print_) {
    ROS_INFO_THROTTLE(2.0, "Chassis acceleration: ax=%.2f m/s^2, ay=%.2f m/s^2, awz=%.2f rad/s^2", 
                     ax_actual_, ay_actual_, awz_actual_);
  }
}

void SentryChassisController::update(const ros::Time &time, const ros::Duration &period) {
  // 更新自锁状态
  updateSelfLockState(time, period);
  
  if (in_self_lock_mode_) {
    // 自锁模式：轮子内扣
    computeWheelCommandsForSelfLock();
  } else {
    // 正常模式：检查指令超时
    bool cmd_vel_active = (time - last_cmd_vel_time_).toSec() < cmd_vel_timeout_;
    
    double vx, vy, wz;
    {
      std::lock_guard<std::mutex> lock(cmd_vel_mutex_);
      
      if (cmd_vel_active) {
        if (world_vel_mode_) {
          geometry_msgs::Twist world_vel, base_vel;
          world_vel.linear.x = vx_;
          world_vel.linear.y = vy_;
          world_vel.angular.z = wz_;
          
          if (transformVelocityToBaseFrame(world_vel, base_vel)) {
            vx = base_vel.linear.x;
            vy = base_vel.linear.y;
            wz = base_vel.angular.z;
            
            if (debug_print_) {
              ROS_INFO_THROTTLE(2.0, "Transformed velocity - World(%.2f,%.2f,%.2f) -> Base(%.2f,%.2f,%.2f)", 
                               vx_, vy_, wz_, vx, vy, wz);
            }
          } else {
            vx = 0.0;
            vy = 0.0;
            wz = 0.0;
            ROS_WARN_THROTTLE(1.0, "TF transform failed, using zero velocity");
          }
        } else {
          vx = vx_;
          vy = vy_;
          wz = wz_;
        }
      } else {
        vx = 0.0;
        vy = 0.0;
        wz = 0.0;
      }
    }
    
    // 使用逆运动学计算轮子命令
    computeWheelCommands(vx, vy, wz);
  }
  
  // 控制轮子速度（添加功率限制）
  double torque_cmd_fl = pid_lf_wheel_.computeCommand(wheel_cmd_[0] - front_left_wheel_joint_.getVelocity(), period);
  double torque_cmd_fr = pid_rf_wheel_.computeCommand(wheel_cmd_[1] - front_right_wheel_joint_.getVelocity(), period);
  double torque_cmd_bl = pid_lb_wheel_.computeCommand(wheel_cmd_[2] - back_left_wheel_joint_.getVelocity(), period);
  double torque_cmd_br = pid_rb_wheel_.computeCommand(wheel_cmd_[3] - back_right_wheel_joint_.getVelocity(), period);
  
  // 获取当前轮子角速度
  double vel_fl = front_left_wheel_joint_.getVelocity();
  double vel_fr = front_right_wheel_joint_.getVelocity();
  double vel_bl = back_left_wheel_joint_.getVelocity();
  double vel_br = back_right_wheel_joint_.getVelocity();
  
  // 应用功率限制
  applyPowerLimits(torque_cmd_fl, torque_cmd_fr, torque_cmd_bl, torque_cmd_br,
                  vel_fl, vel_fr, vel_bl, vel_br);
  
  // 设置扭矩命令
  front_left_wheel_joint_.setCommand(torque_cmd_fl);
  front_right_wheel_joint_.setCommand(torque_cmd_fr);
  back_left_wheel_joint_.setCommand(torque_cmd_bl);
  back_right_wheel_joint_.setCommand(torque_cmd_br);
  
  // 控制转向角度
  front_left_pivot_joint_.setCommand(
      pid_lf_.computeCommand(pivot_cmd_[0] - front_left_pivot_joint_.getPosition(), period));
  front_right_pivot_joint_.setCommand(
      pid_rf_.computeCommand(pivot_cmd_[1] - front_right_pivot_joint_.getPosition(), period));
  back_left_pivot_joint_.setCommand(
      pid_lb_.computeCommand(pivot_cmd_[2] - back_left_pivot_joint_.getPosition(), period));
  back_right_pivot_joint_.setCommand(
      pid_rb_.computeCommand(pivot_cmd_[3] - back_right_pivot_joint_.getPosition(), period));
  
  // 计算并发布里程计
  computeOdometry(time, period);
  
  // 计算并发布加速度
  computeAndPublishAcceleration(time, period);
  
  // 计算并发布功率信息
  computeAndPublishPower(time, period);
  
  //计算并发布轮子状态
  computeAndPublishWheelStates(time, period);
}

PLUGINLIB_EXPORT_CLASS(sentry_chassis_controller::SentryChassisController, controller_interface::ControllerBase)
}