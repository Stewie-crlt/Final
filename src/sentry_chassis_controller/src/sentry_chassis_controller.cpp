#include "sentry_chassis_controller/sentry_chassis_controller.h"
#include <pluginlib/class_list_macros.hpp>
#include <cmath>
#include <tf/transform_listener.h>
#include <Eigen/Dense>

namespace sentry_chassis_controller {

SentryChassisController::SentryChassisController()//初始化
    : vx_(0.0), vy_(0.0), wz_(0.0),
      x_(0.0), y_(0.0), theta_(0.0),
      vx_actual_(0.0), vy_actual_(0.0), wz_actual_(0.0),
      total_distance_(0.0),
      debug_print_(false),
      publish_odom_(false),
      publish_tf_(false),
      odom_frame_id_("odom"),
      base_frame_id_("base_link"),
      world_vel_mode_(false),
      world_frame_id_("odom"),
      enable_lock_mode_(true),
      lock_timeout_(1.0),
      lock_angle_(0.785),
      is_locked_(false) {
}



bool SentryChassisController::init(hardware_interface::EffortJointInterface *effort_joint_interface,ros::NodeHandle &root_nh, ros::NodeHandle &controller_nh) //初始化
{
  // 获取关节句柄
    front_left_wheel_joint_ = effort_joint_interface->getHandle("left_front_wheel_joint");
    front_right_wheel_joint_ = effort_joint_interface->getHandle("right_front_wheel_joint");
    back_left_wheel_joint_ = effort_joint_interface->getHandle("left_back_wheel_joint");
    back_right_wheel_joint_ = effort_joint_interface->getHandle("right_back_wheel_joint");

    front_left_pivot_joint_ = effort_joint_interface->getHandle("left_front_pivot_joint");
    front_right_pivot_joint_ = effort_joint_interface->getHandle("right_front_pivot_joint");
    back_left_pivot_joint_ = effort_joint_interface->getHandle("left_back_pivot_joint");
    back_right_pivot_joint_ = effort_joint_interface->getHandle("right_back_pivot_joint");


  // 获取几何参数
  controller_nh.param("wheel_track", wheel_track_, 0.362);
  controller_nh.param("wheel_base", wheel_base_, 0.362);
  controller_nh.param("wheel_radius", wheel_radius_, 0.07625);
  controller_nh.param("cmd_vel_timeout", cmd_vel_timeout_, 0.5);
  controller_nh.param("max_wheel_speed", max_wheel_speed_, 10.0);
  controller_nh.param("debug_print", debug_print_, false);

  // 里程计
  controller_nh.param("publish_odom", publish_odom_, true);

  //TF参数
  controller_nh.param("publish_tf", publish_tf_, true);
  controller_nh.param("odom_frame_id", odom_frame_id_, std::string("odom"));
  controller_nh.param("base_frame_id", base_frame_id_, std::string("base_link"));

  // 控制模式选择参数
  controller_nh.param("world_vel_mode", world_vel_mode_, false);
  controller_nh.param("world_frame_id", world_frame_id_, std::string("odom"));

  // 自锁功能参数
  controller_nh.param("enable_lock_mode", enable_lock_mode_, true);
  controller_nh.param("lock_timeout", lock_timeout_, 1.0);
  controller_nh.param("lock_angle", lock_angle_, 0.785);

  // 路程发布参数
  bool publish_distance;
  std::string distance_topic;
  controller_nh.param("publish_distance", publish_distance, true);
  controller_nh.param("distance_topic", distance_topic, std::string("total_distance"));

  // 从参数服务器获取初始PID参数
  double pivot_p, pivot_i, pivot_d;
  double wheel_p, wheel_i, wheel_d;
  
  controller_nh.param("pivot_p", pivot_p, 5.0);
  controller_nh.param("pivot_i", pivot_i, 1.0);
  controller_nh.param("pivot_d", pivot_d, 0.1);
  
  controller_nh.param("wheel_p", wheel_p, 2.0);
  controller_nh.param("wheel_i", wheel_i, 0.1);
  controller_nh.param("wheel_d", wheel_d, 0.0);

  // 初始化所有PID控制器
  pid_lf_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_rf_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_lb_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_rb_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  
  pid_lf_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);
  pid_rf_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);
  pid_lb_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);
  pid_rb_wheel_.initPid(wheel_p, wheel_i, wheel_d, 0.0, 0.0);

  // 初始化速度指令
  vx_ = 0.0;
  vy_ = 0.0;
  wz_ = 0.0;
  last_cmd_vel_time_ = ros::Time::now();

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
  
  // 初始化总路程
  total_distance_ = 0.0;
  
  // 订阅cmd_vel话题
  ros::NodeHandle nh;
  cmd_vel_sub_ = nh.subscribe<geometry_msgs::Twist>("cmd_vel", 1, &SentryChassisController::cmdVelCallback, this);

  // 发布里程计话题
  if (publish_odom_) {
    odom_pub_ = nh.advertise<nav_msgs::Odometry>("odom", 50);
  }

  // 发布路程话题
  if (publish_distance) {
    distance_pub_ = nh.advertise<std_msgs::Float32>(distance_topic, 10);
  }

  // 初始化TF监听器（用于世界坐标系速度模式）
  if (world_vel_mode_) {
    tf_listener_.reset(new tf::TransformListener());
  }

  // 初始化TF广播器
  if (publish_tf_) {
    tf_broadcaster_.reset(new tf::TransformBroadcaster());
  }

  // 设置动态重配置服务器
  dyn_reconf_server_.reset(
      new dynamic_reconfigure::Server<sentry_chassis_controller::PIDConfig>(controller_nh));
  
  dynamic_reconfigure::Server<sentry_chassis_controller::PIDConfig>::CallbackType cb =
      boost::bind(&SentryChassisController::reconfigureCallback, this, _1, _2);
  dyn_reconf_server_->setCallback(cb);

  ROS_INFO("SentryChassisController initialized successfully");
  ROS_INFO("  Wheel track: %.3f m, Wheel base: %.3f m, Wheel radius: %.3f m", 
           wheel_track_, wheel_base_, wheel_radius_);
  ROS_INFO("  World velocity mode: %s", world_vel_mode_ ? "enabled" : "disabled");
  ROS_INFO("  Lock mode: %s (timeout: %.1f s, angle: %.2f rad)", 
           enable_lock_mode_ ? "enabled" : "disabled", lock_timeout_, lock_angle_);
  ROS_INFO("  Initial Pivot PID: P=%.2f, I=%.2f, D=%.2f", pivot_p, pivot_i, pivot_d);
  ROS_INFO("  Initial Wheel PID: P=%.2f, I=%.2f, D=%.2f", wheel_p, wheel_i, wheel_d);

  return true;
}

// 动态重配置回调函数
void SentryChassisController::reconfigureCallback(sentry_chassis_controller::PIDConfig &config, uint32_t level) {
  // 设置转向PID参数
  pid_lf_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
  pid_rf_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
  pid_lb_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
  pid_rb_.setGains(config.pivot_p, config.pivot_i, config.pivot_d, 0.0, 0.0);
  
  // 设置驱动PID参数
  pid_lf_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
  pid_rf_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
  pid_lb_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
  pid_rb_wheel_.setGains(config.wheel_p, config.wheel_i, config.wheel_d, 0.0, 0.0);
  
  ROS_INFO_THROTTLE(1.0, "PID parameters updated: "
                   "Pivot(P=%.2f, I=%.2f, D=%.2f), "
                   "Wheel(P=%.2f, I=%.2f, D=%.2f)",
                   config.pivot_p, config.pivot_i, config.pivot_d,
                   config.wheel_p, config.wheel_i, config.wheel_d);
}


bool SentryChassisController::transformVelocityToBaseFrame(const geometry_msgs::Twist& world_vel, geometry_msgs::Twist& base_vel) //TF转化函数
{
  if (!tf_listener_) {
    ROS_WARN_THROTTLE(1.0, "TF listener not initialized");
    return false;
  }
  
    tf::StampedTransform transform;
    tf_listener_->lookupTransform(world_frame_id_, base_frame_id_, ros::Time(0), transform);
    
    double yaw = tf::getYaw(transform.getRotation());
    
    base_vel.linear.x = world_vel.linear.x * cos(yaw) + world_vel.linear.y * sin(yaw);
    base_vel.linear.y = -world_vel.linear.x * sin(yaw) + world_vel.linear.y * cos(yaw);
    base_vel.angular.z = world_vel.angular.z;
    
    return true;
 
}

void SentryChassisController::setLockMode(bool enable) //设置自锁函数
{
  if (enable == is_locked_) {
    return;
  }
  
  if (enable) {
    ROS_INFO("Entering lock mode");
    
    pivot_cmd_[0] = lock_angle_;
    pivot_cmd_[1] = -lock_angle_;
    pivot_cmd_[2] = -lock_angle_;
    pivot_cmd_[3] = lock_angle_;
    
    for (int i = 0; i < 4; ++i) {
      wheel_cmd_[i] = 0.0;
      last_valid_pivot_cmd_[i] = pivot_cmd_[i];
    }
    
    pid_lf_.reset();
    pid_rf_.reset();
    pid_lb_.reset();
    pid_rb_.reset();
    pid_lf_wheel_.reset();
    pid_rf_wheel_.reset();
    pid_lb_wheel_.reset();
    pid_rb_wheel_.reset();
    
    is_locked_ = true;
    
    if (debug_print_) {
      ROS_INFO("Lock mode activated: angles = [%.2f, %.2f, %.2f, %.2f] rad", 
               pivot_cmd_[0], pivot_cmd_[1], pivot_cmd_[2], pivot_cmd_[3]);
    }
  } else {
    ROS_INFO("Exiting lock mode");
    
    pid_lf_.reset();
    pid_rf_.reset();
    pid_lb_.reset();
    pid_rb_.reset();
    pid_lf_wheel_.reset();
    pid_rf_wheel_.reset();
    pid_lb_wheel_.reset();
    pid_rb_wheel_.reset();
    
    is_locked_ = false;
  }
}

void SentryChassisController::starting(const ros::Time& time) {
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
  is_locked_ = false;
  total_distance_ = 0.0;
  
  if (distance_pub_) {
    std_msgs::Float32 distance_msg;
    distance_msg.data = total_distance_;
    distance_pub_.publish(distance_msg);
  }
  
  ROS_INFO("SentryChassisController started. Total distance reset to 0.");
}

void SentryChassisController::stopping(const ros::Time& time) //设置停止状态
{
  if (enable_lock_mode_) {
    setLockMode(true);
    
    front_left_wheel_joint_.setCommand(0.0);
    front_right_wheel_joint_.setCommand(0.0);
    back_left_wheel_joint_.setCommand(0.0);
    back_right_wheel_joint_.setCommand(0.0);
    
    ROS_INFO("Controller stopping. Final total distance: %.3f meters", total_distance_);
  } else {
    front_left_wheel_joint_.setCommand(0.0);
    front_right_wheel_joint_.setCommand(0.0);
    back_left_wheel_joint_.setCommand(0.0);
    back_right_wheel_joint_.setCommand(0.0);
    
    ROS_INFO("Controller stopping. Final total distance: %.3f meters", total_distance_);
  }
}

void SentryChassisController::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) 
{
  std::lock_guard<std::mutex> lock(cmd_vel_mutex_);
  
  if (is_locked_) {
    setLockMode(false);
  }
  
  vx_ = msg->linear.x;
  vy_ = msg->linear.y;
  wz_ = msg->angular.z;
  
  last_cmd_vel_time_ = ros::Time::now();
  
  if (debug_print_) {
    ROS_INFO_THROTTLE(2.0, "Received cmd_vel: vx=%.2f, vy=%.2f, wz=%.2f", vx_, vy_, wz_);
  }
}

void SentryChassisController::computeWheelCommands(double vx, double vy, double wz) {
  double L = wheel_base_ / 2.0;
  double W = wheel_track_ / 2.0;
  double R = wheel_radius_;
  
  // 前左轮位置: (L, W)
  double vx_fl = vx - wz * W;
  double vy_fl = vy + wz * L;
  
  // 前右轮位置: (L, -W)
  double vx_fr = vx - wz * (-W);
  double vy_fr = vy + wz * L;
  
  // 后左轮位置: (-L, W)
  double vx_bl = vx - wz * W;
  double vy_bl = vy + wz * (-L);
  
  // 后右轮位置: (-L, -W)
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
  
  double total_speed = fabs(vx) + fabs(vy) + fabs(wz);
  if (total_speed < 0.01) {
    for (int i = 0; i < 4; ++i) {
      wheel_cmd_[i] = 0.0;
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

void SentryChassisController::computeOdometry(const ros::Time &time, const ros::Duration &period)//正运动学实现里程计
 {
  double wheel_vel[4] = {
    front_left_wheel_joint_.getVelocity(),
    front_right_wheel_joint_.getVelocity(),
    back_left_wheel_joint_.getVelocity(),
    back_right_wheel_joint_.getVelocity()
  };
  
  double pivot_angle[4] = {
    front_left_pivot_joint_.getPosition(),
    front_right_pivot_joint_.getPosition(),
    back_left_pivot_joint_.getPosition(),
    back_right_pivot_joint_.getPosition()
  };
  
  double L = wheel_base_ / 2.0;
  double W = wheel_track_ / 2.0;
  double R = wheel_radius_;
  
  // 计算每个轮子的线速度
  double wheel_linear_v[4];
  for (int i = 0; i < 4; ++i) {
    wheel_linear_v[i] = wheel_vel[i] * R;
  }
  
  // 构建雅可比矩阵
  // 对于每个轮子，速度与底盘速度的关系为：
  // v_wheel_i = J_i * [vx, vy, wz]^T
  // 其中J_i = [cos(theta_i), sin(theta_i), -L_i*sin(theta_i) + W_i*cos(theta_i)]
  
  Eigen::MatrixXd J(4, 3);  // 雅可比矩阵
  Eigen::VectorXd v_wheel(4);  // 轮子线速度向量
  
  // 轮子位置 (x, y) 在底盘坐标系中
  // 前左: (L, W), 前右: (L, -W), 后左: (-L, W), 后右: (-L, -W)
  double wheel_pos[4][2] = {
    {L, W},    // 前左
    {L, -W},   // 前右
    {-L, W},   // 后左
    {-L, -W}   // 后右
  };
  
  for (int i = 0; i < 4; ++i) {
    double theta_i = pivot_angle[i];  // 轮子转向角
    double x_i = wheel_pos[i][0];
    double y_i = wheel_pos[i][1];
    
    // 雅可比矩阵的行
    J(i, 0) = cos(theta_i);
    J(i, 1) = sin(theta_i);
    J(i, 2) = -x_i * sin(theta_i) + y_i * cos(theta_i);
    
    // 轮子线速度
    v_wheel(i) = wheel_linear_v[i];
  }
  
  // 使用最小二乘法求解底盘速度 [vx, vy, wz]^T
  // (J^T * J) * V = J^T * v_wheel
  // V = (J^T * J)^(-1) * J^T * v_wheel
  
  Eigen::MatrixXd J_T = J.transpose();//转置
  Eigen::MatrixXd J_T_J = J_T * J;
  
  // 检查雅可比矩阵是否满秩（避免奇异矩阵）
  double det = J_T_J.determinant();
  if (fabs(det) > 1e-6) {
    // 正常情况：雅可比矩阵可逆
    Eigen::MatrixXd J_pseudoinv = J_T_J.inverse() * J_T;
    Eigen::VectorXd V = J_pseudoinv * v_wheel;
    
    vx_actual_ = V(0);
    vy_actual_ = V(1);
    wz_actual_ = V(2);
  } else {
    // 奇异情况：使用简单平均作为备用方案
    ROS_WARN_THROTTLE(1.0, "Jacobian matrix is singular, using fallback method");
    
    // 计算轮子在底盘坐标系中的速度分量
    double vx_wheel[4], vy_wheel[4];
    for (int i = 0; i < 4; ++i) {
      double theta_i = pivot_angle[i];
      vx_wheel[i] = wheel_linear_v[i] * cos(theta_i);
      vy_wheel[i] = wheel_linear_v[i] * sin(theta_i);
    }
    
    // 简单平均
    vx_actual_ = (vx_wheel[0] + vx_wheel[1] + vx_wheel[2] + vx_wheel[3]) / 4.0;
    vy_actual_ = (vy_wheel[0] + vy_wheel[1] + vy_wheel[2] + vy_wheel[3]) / 4.0;
    
    // 计算角速度
    wz_actual_ = 0.0;
    double sum_wz = 0.0;
    int count = 0;
    
    for (int i = 0; i < 4; ++i) {
      double wheel_vx = vx_wheel[i];
      double wheel_vy = vy_wheel[i];
      double wheel_x = wheel_pos[i][0];
      double wheel_y = wheel_pos[i][1];
      
      double wz_cb = ((wheel_vx - vx_actual_) * (-wheel_y) + 
                     (wheel_vy - vy_actual_) * wheel_x) / (wheel_x * wheel_x + wheel_y * wheel_y);
      
      if (!std::isnan(wz_cb) && !std::isinf(wz_cb)) {
        sum_wz += wz_cb;
        count++;
      }
    }
    
    if (count > 0) {
      wz_actual_ = sum_wz / count;
    }
  }
  
  double dt = period.toSec();
  if (dt > 0.0 && dt < 1.0) {
    // 更新姿态
    theta_ += wz_actual_ * dt;
    
    // 规范化角度到 [-π, π]
    while (theta_ > M_PI) theta_ -= 2.0 * M_PI;
    while (theta_ < -M_PI) theta_ += 2.0 * M_PI;
    
    // 将底盘坐标系速度转换到世界坐标系
    double world_vx = vx_actual_ * cos(theta_) - vy_actual_ * sin(theta_);
    double world_vy = vx_actual_ * sin(theta_) + vy_actual_ * cos(theta_);
    
    // 更新位置
    x_ += world_vx * dt;
    y_ += world_vy * dt;
    
    // 计算并累加总路程
    double instant_speed = sqrt(world_vx * world_vx + world_vy * world_vy);
    total_distance_ += instant_speed * dt;
    
    // 每5秒打印一次总路程
    static ros::Time last_distance_print = time;
    if ((time - last_distance_print).toSec() > 5.0) {
      ROS_INFO_THROTTLE(5.0, "Total distance traveled: %.3f meters", total_distance_);
      last_distance_print = time;
    }
  }
  
  // 发布里程计消息
  if (publish_odom_ && odom_pub_) {
    nav_msgs::Odometry odom;
    odom.header.stamp = time;
    odom.header.frame_id = odom_frame_id_;
    odom.child_frame_id = base_frame_id_;
    
    odom.pose.pose.position.x = x_;
    odom.pose.pose.position.y = y_;
    odom.pose.pose.position.z = 0.0;
    
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(theta_);
    odom.pose.pose.orientation = odom_quat;
    
    odom.twist.twist.linear.x = vx_actual_;
    odom.twist.twist.linear.y = vy_actual_;
    odom.twist.twist.angular.z = wz_actual_;
    
    // 添加协方差（可选的，但推荐）
    // 位置协方差（假设x,y,z独立，z固定为0）
    odom.pose.covariance[0] = 0.01;  // x方差
    odom.pose.covariance[7] = 0.01;  // y方差
    odom.pose.covariance[35] = 0.01; // 偏航角方差
    
    // 速度协方差
    odom.twist.covariance[0] = 0.01;  // vx方差
    odom.twist.covariance[7] = 0.01;  // vy方差
    odom.twist.covariance[35] = 0.01; // wz方差
    
    odom_pub_.publish(odom);
  }
  
  // 发布总路程
  if (distance_pub_) {
    std_msgs::Float32 distance_msg;
    distance_msg.data = total_distance_;
    distance_pub_.publish(distance_msg);
  }
  
  // 发布TF变换
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

void SentryChassisController::update(const ros::Time &time, const ros::Duration &period) {
  bool cmd_vel_active = (time - last_cmd_vel_time_).toSec() < cmd_vel_timeout_;
  
  if (enable_lock_mode_) {
    bool should_lock = (time - last_cmd_vel_time_).toSec() > lock_timeout_;
    if (should_lock && !is_locked_) {
      setLockMode(true);
    }
  }
  
  double vx, vy, wz;
  {
    std::lock_guard<std::mutex> lock(cmd_vel_mutex_);
    
    if (cmd_vel_active && !is_locked_) {
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
    } else if (is_locked_) {
      vx = 0.0;
      vy = 0.0;
      wz = 0.0;
    } else {
      vx = 0.0;
      vy = 0.0;
      wz = 0.0;
    }
  }
  
  if (!is_locked_) {
    computeWheelCommands(vx, vy, wz);
  }
  
  if (!is_locked_) {
    front_left_wheel_joint_.setCommand(
        pid_lf_wheel_.computeCommand(wheel_cmd_[0] - front_left_wheel_joint_.getVelocity(), period));
    front_right_wheel_joint_.setCommand(
        pid_rf_wheel_.computeCommand(wheel_cmd_[1] - front_right_wheel_joint_.getVelocity(), period));
    back_left_wheel_joint_.setCommand(
        pid_lb_wheel_.computeCommand(wheel_cmd_[2] - back_left_wheel_joint_.getVelocity(), period));
    back_right_wheel_joint_.setCommand(
        pid_rb_wheel_.computeCommand(wheel_cmd_[3] - back_right_wheel_joint_.getVelocity(), period));
  } else {
    front_left_wheel_joint_.setCommand(0.0);
    front_right_wheel_joint_.setCommand(0.0);
    back_left_wheel_joint_.setCommand(0.0);
    back_right_wheel_joint_.setCommand(0.0);
  }
  
  front_left_pivot_joint_.setCommand(
      pid_lf_.computeCommand(pivot_cmd_[0] - front_left_pivot_joint_.getPosition(), period));
  front_right_pivot_joint_.setCommand(
      pid_rf_.computeCommand(pivot_cmd_[1] - front_right_pivot_joint_.getPosition(), period));
  back_left_pivot_joint_.setCommand(
      pid_lb_.computeCommand(pivot_cmd_[2] - back_left_pivot_joint_.getPosition(), period));
  back_right_pivot_joint_.setCommand(
      pid_rb_.computeCommand(pivot_cmd_[3] - back_right_pivot_joint_.getPosition(), period));
  
  computeOdometry(time, period);
}

PLUGINLIB_EXPORT_CLASS(sentry_chassis_controller::SentryChassisController, controller_interface::ControllerBase)

} // namespace sentry_chassis_controller