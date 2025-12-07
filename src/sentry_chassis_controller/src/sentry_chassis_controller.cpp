#include "sentry_chassis_controller/sentry_chassis_controller.h"
#include <pluginlib/class_list_macros.hpp>
#include <cmath>
#include <tf/transform_listener.h>

namespace sentry_chassis_controller {

bool SentryChassisController::init(hardware_interface::EffortJointInterface *effort_joint_interface,
                                   ros::NodeHandle &root_nh, ros::NodeHandle &controller_nh) {
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
  wheel_radius_ = controller_nh.param("wheel_radius", 0.07625);
  cmd_vel_timeout_ = controller_nh.param("cmd_vel_timeout", 0.5);
  max_wheel_speed_ = controller_nh.param("max_wheel_speed", 10.0); // rad/s
  debug_print_ = controller_nh.param("debug_print", false);
  publish_odom_ = controller_nh.param("publish_odom", true);
  publish_tf_ = controller_nh.param("publish_tf", true);
  odom_frame_id_ = controller_nh.param("odom_frame_id", std::string("odom"));
  base_frame_id_ = controller_nh.param("base_frame_id", std::string("base_link"));
  
  // 新增：控制模式参数
  world_vel_mode_ = controller_nh.param("world_vel_mode", false);
  world_frame_id_ = controller_nh.param("world_frame_id", std::string("odom"));

  // 新增：自锁功能参数
  enable_lock_mode_ = controller_nh.param("enable_lock_mode", true);
  lock_timeout_ = controller_nh.param("lock_timeout", 1.0);
  lock_angle_f = controller_nh.param("lock_angle", 0.785);
  lock_angle_b = controller_nh.param("lock_angle", 2.355); // 45度，单位弧度
  is_locked_ = false;

  // 新增：路程发布参数
  bool publish_distance = controller_nh.param("publish_distance", true);
  std::string distance_topic = controller_nh.param("distance_topic", std::string("total_distance"));

  // 初始化PID控制器
  // 转向角度PID
  double pivot_p, pivot_i, pivot_d;
  controller_nh.param("pivot_p", pivot_p, 5.0);
  controller_nh.param("pivot_i", pivot_i, 0.0);
  controller_nh.param("pivot_d", pivot_d, 0.1);
  
  pid_lf_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_rf_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_lb_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);
  pid_rb_.initPid(pivot_p, pivot_i, pivot_d, 0.0, 0.0);

  // 轮子速度控制的PID
  double wheel_p, wheel_i, wheel_d;
  controller_nh.param("wheel_p", wheel_p, 2.0);
  controller_nh.param("wheel_i", wheel_i, 0.1);
  controller_nh.param("wheel_d", wheel_d, 0.0);
  
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
  
  // 新增：初始化总路程
  total_distance_ = 0.0;

  // 初始化TF监听器（用于世界坐标系速度模式）
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

  // 新增：发布路程话题
  if (publish_distance) {
    distance_pub_ = nh.advertise<std_msgs::Float32>(distance_topic, 10);
  }

  // 初始化TF广播器
  if (publish_tf_) {
    tf_broadcaster_.reset(new tf::TransformBroadcaster());
  }

  ROS_INFO("SentryChassisController initialized: world_vel_mode = %s, enable_lock_mode = %s, lock_anglef = %.2f rad, lock_angleb = %.2f rad", 
           world_vel_mode_ ? "true" : "false", 
           enable_lock_mode_ ? "true" : "false",
           lock_angle_f,lock_angle_b);
  ROS_INFO("Distance tracking: %s", publish_distance ? "enabled" : "disabled");

  return true;
}

// 新增：将世界坐标系速度转换到底盘坐标系
bool SentryChassisController::transformVelocityToBaseFrame(const geometry_msgs::Twist& world_vel,
                                                           geometry_msgs::Twist& base_vel) {
  try {
    tf::StampedTransform transform;
    tf_listener_->lookupTransform(world_frame_id_, base_frame_id_, ros::Time(0), transform);
    
    // 获取底盘在世界坐标系中的偏航角
    double yaw = tf::getYaw(transform.getRotation());
    
    // 线速度变换：世界坐标系 -> 底盘坐标系
    base_vel.linear.x = world_vel.linear.x * cos(yaw) + world_vel.linear.y * sin(yaw);
    base_vel.linear.y = -world_vel.linear.x * sin(yaw) + world_vel.linear.y * cos(yaw);
    
    // 角速度在z轴方向不变（假设世界坐标系和底盘坐标系的z轴平行）
    base_vel.angular.z = world_vel.angular.z;
    
    return true;
  }
  catch (tf::TransformException &ex) {
    ROS_WARN_THROTTLE(1.0, "TF transform failed: %s", ex.what());
    return false;
  }
}

// 新增：设置自锁模式
void SentryChassisController::setLockMode(bool enable) {
  if (enable == is_locked_) {
    return; // 状态相同，无需改变
  }
  
  if (enable) {
    // 进入自锁模式
    ROS_INFO("Entering lock mode");
    
    // 设置轮子转向自锁角度
    // 左侧轮子：lock_angle（正角度，向外）
    // 右侧轮子：-lock_angle（负角度，向外）
    // 这样所有轮子都向外转，形成自锁
    pivot_cmd_[0] = lock_angle_f;      // 左前：向外转
    pivot_cmd_[1] = -lock_angle_f;     // 右前：向外转  
    pivot_cmd_[2] = -lock_angle_f;      // 左后：向外转
    pivot_cmd_[3] = lock_angle_f;     // 右后：向外转
    
    // 所有轮子速度设为0
    for (int i = 0; i < 4; ++i) {
      wheel_cmd_[i] = 0.0;
      last_valid_pivot_cmd_[i] = pivot_cmd_[i];
    }
    
    // 重置PID控制器，避免积分项累积
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
    // 退出自锁模式
    ROS_INFO("Exiting lock mode");
    
    // 重置PID控制器
    pid_lf_.reset();
    pid_rf_.reset();
    pid_lb_.reset();
    pid_rb_.reset();
    
    is_locked_ = false;
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
  
  // 初始化为非自锁状态
  is_locked_ = false;
  
  // 重置总路程
  total_distance_ = 0.0;
  
  ROS_INFO("SentryChassisController started. Total distance reset to 0.");
}

void SentryChassisController::stopping(const ros::Time& time) {
  // 停止时进入自锁模式
  if (enable_lock_mode_) {
    setLockMode(true);
    
    // 设置所有轮子速度为0
    front_left_wheel_joint_.setCommand(0.0);
    front_right_wheel_joint_.setCommand(0.0);
    back_left_wheel_joint_.setCommand(0.0);
    back_right_wheel_joint_.setCommand(0.0);
    
    ROS_INFO("Controller stopping. Final total distance: %.3f meters", total_distance_);
  } else {
    // 停止时设置所有轮子速度为0
    front_left_wheel_joint_.setCommand(0.0);
    front_right_wheel_joint_.setCommand(0.0);
    back_left_wheel_joint_.setCommand(0.0);
    back_right_wheel_joint_.setCommand(0.0);
    
    ROS_INFO("Controller stopping. Final total distance: %.3f meters", total_distance_);
  }
}

void SentryChassisController::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
  std::lock_guard<std::mutex> lock(cmd_vel_mutex_);
  
  // 如果当前处于自锁模式，收到速度命令时退出自锁模式
  if (is_locked_) {
    setLockMode(false);
  }
  
  if (world_vel_mode_) {
    // 世界坐标系速度模式：直接存储世界坐标系速度
    vx_ = msg->linear.x;
    vy_ = msg->linear.y;
    wz_ = msg->angular.z;
  } else {
    // 底盘坐标系速度模式：直接使用指令速度
    vx_ = msg->linear.x;
    vy_ = msg->linear.y;
    wz_ = msg->angular.z;
  }
  
  last_cmd_vel_time_ = ros::Time::now();
  
  if (debug_print_) {
    if (world_vel_mode_) {
      ROS_INFO_THROTTLE(1.0, "Received world velocity command: vx=%.2f, vy=%.2f, wz=%.2f", 
                       vx_, vy_, wz_);
    } else {
      ROS_INFO_THROTTLE(1.0, "Received base velocity command: vx=%.2f, vy=%.2f, wz=%.2f", 
                       vx_, vy_, wz_);
    }
  }
}

void SentryChassisController::computeWheelCommands(double vx, double vy, double wz) {
  // 全向移动底盘逆运动学计算
  // 轮子顺序: 0-前左, 1-前右, 2-后左, 3-后右
  
  // 底盘几何参数
  double L = wheel_base_ / 2.0;  // 前后轮距的一半
  double W = wheel_track_ / 2.0; // 左右轮距的一半
  double R = wheel_radius_;      // 轮子半径
  
  // 计算每个轮子的速度向量
  // 基于底盘运动学模型:
  // V_wheel = [vx - wz * Y, vy + wz * X] 
  // 其中 (X, Y) 是轮子相对于底盘中心的位置
  
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
  
  // 计算每个轮子的转向角度 (相对于底盘X轴)
  double new_pivot_cmd[4];
  new_pivot_cmd[0] = atan2(vy_fl, vx_fl);  // 前左轮
  new_pivot_cmd[1] = atan2(vy_fr, vx_fr);  // 前右轮
  new_pivot_cmd[2] = atan2(vy_bl, vx_bl);  // 后左轮
  new_pivot_cmd[3] = atan2(vy_br, vx_br);  // 后右轮
  
  // 计算每个轮子的线速度大小
  double speed_fl = sqrt(vx_fl * vx_fl + vy_fl * vy_fl);
  double speed_fr = sqrt(vx_fr * vx_fr + vy_fr * vy_fr);
  double speed_bl = sqrt(vx_bl * vx_bl + vy_bl * vy_bl);
  double speed_br = sqrt(vx_br * vx_br + vy_br * vy_br);
  
  // 将线速度转换为角速度 (rad/s)
  wheel_cmd_[0] = speed_fl / R;
  wheel_cmd_[1] = speed_fr / R;
  wheel_cmd_[2] = speed_bl / R;
  wheel_cmd_[3] = speed_br / R;
  
  // 限制最大轮子速度
  for (int i = 0; i < 4; ++i) {
    if (fabs(wheel_cmd_[i]) > max_wheel_speed_) {
      wheel_cmd_[i] = (wheel_cmd_[i] > 0) ? max_wheel_speed_ : -max_wheel_speed_;
    }
  }
  
  // 处理零速度情况 - 保持当前转向角度
  double total_speed = fabs(vx) + fabs(vy) + fabs(wz);
  if (total_speed < 0.01) {
    for (int i = 0; i < 4; ++i) {
      wheel_cmd_[i] = 0.0;
      // 使用上一次有效的转向角度
      pivot_cmd_[i] = last_valid_pivot_cmd_[i];
    }
  } else {
    // 更新转向角度并记录为有效角度
    for (int i = 0; i < 4; ++i) {
      pivot_cmd_[i] = new_pivot_cmd[i];
      last_valid_pivot_cmd_[i] = new_pivot_cmd[i];
    }
  }
  
  // 调试输出
  if (debug_print_) {
    ROS_INFO_THROTTLE(1.0, 
      "Wheel commands - FL(angle: %.2f, speed: %.2f) FR(angle: %.2f, speed: %.2f) "
      "BL(angle: %.2f, speed: %.2f) BR(angle: %.2f, speed: %.2f)", 
      pivot_cmd_[0], wheel_cmd_[0], pivot_cmd_[1], wheel_cmd_[1],
      pivot_cmd_[2], wheel_cmd_[2], pivot_cmd_[3], wheel_cmd_[3]);
  }
}

void SentryChassisController::computeOdometry(const ros::Time &time, const ros::Duration &period) {
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
  
  // 底盘几何参数
  double L = wheel_base_ / 2.0;  // 前后轮距的一半
  double W = wheel_track_ / 2.0; // 左右轮距的一半
  double R = wheel_radius_;      // 轮子半径
  
  // 将轮子角速度转换为线速度
  double wheel_linear_vel[4];
  for (int i = 0; i < 4; ++i) {
    wheel_linear_vel[i] = wheel_vel[i] * R;
  }
  
  // 正运动学：根据轮子速度和角度计算机体速度
  // 每个轮子在机体坐标系下的速度分量
  double vx_wheel[4], vy_wheel[4];
  for (int i = 0; i < 4; ++i) {
    vx_wheel[i] = wheel_linear_vel[i] * cos(pivot_angle[i]);
    vy_wheel[i] = wheel_linear_vel[i] * sin(pivot_angle[i]);
  }
  
  // 使用加权平均计算机体速度
  // 对于全向底盘，我们可以直接对轮子速度进行平均
  vx_actual_ = (vx_wheel[0] + vx_wheel[1] + vx_wheel[2] + vx_wheel[3]) / 4.0;
  vy_actual_ = (vy_wheel[0] + vy_wheel[1] + vy_wheel[2] + vy_wheel[3]) / 4.0;
  
  // 计算角速度 - 使用运动学关系
  wz_actual_ = 0.0;
  double sum_wz = 0.0;
  int count = 0;
  
  // 通过每个轮子的运动关系估算角速度
  for (int i = 0; i < 4; ++i) {
    // 根据轮子位置和速度计算对机体角速度的贡献
    double wheel_vx = vx_wheel[i];
    double wheel_vy = vy_wheel[i];
    
    // 轮子位置
    double wheel_x, wheel_y;
    switch(i) {
      case 0: // 前左
        wheel_x = L; wheel_y = W;
        break;
      case 1: // 前右
        wheel_x = L; wheel_y = -W;
        break;
      case 2: // 后左
        wheel_x = -L; wheel_y = W;
        break;
      case 3: // 后右
        wheel_x = -L; wheel_y = -W;
        break;
    }
    
    // 轮子速度应该等于机体速度加上角速度引起的线速度
    // v_wheel = v_body + w × r
    // 所以：w = (v_wheel - v_body) × r / |r|^2 在z方向的分量
    double wz_contrib = ((wheel_vx - vx_actual_) * (-wheel_y) + 
                        (wheel_vy - vy_actual_) * wheel_x) / (wheel_x * wheel_x + wheel_y * wheel_y);
    
    if (!std::isnan(wz_contrib) && !std::isinf(wz_contrib)) {
      sum_wz += wz_contrib;
      count++;
    }
  }
  
  if (count > 0) {
    wz_actual_ = sum_wz / count;
  }
  
  // 积分得到位置
  double dt = period.toSec();
  if (dt > 0.0) {
    // 更新朝向
    theta_ += wz_actual_ * dt;
    
    // 标准化角度到 [-pi, pi]
    while (theta_ > M_PI) theta_ -= 2.0 * M_PI;
    while (theta_ < -M_PI) theta_ += 2.0 * M_PI;
    
    // 在世界坐标系下的速度
    double world_vx = vx_actual_ * cos(theta_) - vy_actual_ * sin(theta_);
    double world_vy = vx_actual_ * sin(theta_) + vy_actual_ * cos(theta_);
    
    // 更新位置（位移）
    x_ += world_vx * dt;
    y_ += world_vy * dt;
    
    // 新增：计算并累加路程
    double instant_speed = sqrt(world_vx * world_vx + world_vy * world_vy);
    total_distance_ += instant_speed * dt;
    
    // 调试输出：每5秒打印一次总路程
    static ros::Time last_distance_print = time;
    if ((time - last_distance_print).toSec() > 5.0) {
      ROS_INFO_THROTTLE(5.0, "Total distance traveled: %.3f meters", total_distance_);
      last_distance_print = time;
    }
  }
  
  // 发布里程计消息
  if (publish_odom_ && odom_pub_.getNumSubscribers() > 0) {
    nav_msgs::Odometry odom;
    odom.header.stamp = time;
    odom.header.frame_id = odom_frame_id_;
    odom.child_frame_id = base_frame_id_;
    
    // 设置位置
    odom.pose.pose.position.x = x_;
    odom.pose.pose.position.y = y_;
    odom.pose.pose.position.z = 0.0;
    
    // 设置朝向（四元数）
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(theta_);
    odom.pose.pose.orientation = odom_quat;
    
    // 设置速度
    odom.twist.twist.linear.x = vx_actual_;
    odom.twist.twist.linear.y = vy_actual_;
    odom.twist.twist.angular.z = wz_actual_;
    
    // 发布里程计
    odom_pub_.publish(odom);
  }
  
  // 新增：发布路程消息
  if (distance_pub_.getNumSubscribers() > 0) {
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
  // 检查指令超时
  bool cmd_vel_active = (time - last_cmd_vel_time_).toSec() < cmd_vel_timeout_;
  
  // 检查是否应该进入自锁模式
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
      // 如果指令有效且不在自锁模式
      if (world_vel_mode_) {
        // 世界坐标系速度模式：需要转换到底盘坐标系
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
          // TF变换失败，使用零速度
          vx = 0.0;
          vy = 0.0;
          wz = 0.0;
          ROS_WARN_THROTTLE(1.0, "TF transform failed, using zero velocity");
        }
      } else {
        // 底盘坐标系速度模式：直接使用指令速度
        vx = vx_;
        vy = vy_;
        wz = wz_;
      }
    } else if (is_locked_) {
      // 自锁模式下，速度设为0
      vx = 0.0;
      vy = 0.0;
      wz = 0.0;
      
      // 自锁模式下不重新计算轮子命令，保持自锁角度
      // 直接使用setLockMode设置的角度
    } else {
      // 指令超时但未进入自锁模式，使用零速度
      vx = 0.0;
      vy = 0.0;
      wz = 0.0;
    }
  }
  
  // 只有在非自锁模式下才重新计算轮子命令
  if (!is_locked_) {
    // 使用逆运动学计算轮子命令
    computeWheelCommands(vx, vy, wz);
  }
  
  // 控制轮子速度
  front_left_wheel_joint_.setCommand(
      pid_lf_wheel_.computeCommand(wheel_cmd_[0] - front_left_wheel_joint_.getVelocity(), period));
  front_right_wheel_joint_.setCommand(
      pid_rf_wheel_.computeCommand(wheel_cmd_[1] - front_right_wheel_joint_.getVelocity(), period));
  back_left_wheel_joint_.setCommand(
      pid_lb_wheel_.computeCommand(wheel_cmd_[2] - back_left_wheel_joint_.getVelocity(), period));
  back_right_wheel_joint_.setCommand(
      pid_rb_wheel_.computeCommand(wheel_cmd_[3] - back_right_wheel_joint_.getVelocity(), period));
  
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
}

PLUGINLIB_EXPORT_CLASS(sentry_chassis_controller::SentryChassisController, controller_interface::ControllerBase)
}