#ifndef SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H
#define SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H

#include <ros/ros.h>
#include <controller_interface/controller.h>
#include <hardware_interface/joint_command_interface.h>
#include <control_toolbox/pid.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>
#include <mutex>

namespace sentry_chassis_controller {

class SentryChassisController : public controller_interface::Controller<hardware_interface::EffortJointInterface> {
 public:
  SentryChassisController() = default;
  ~SentryChassisController() override = default;

  bool init(hardware_interface::EffortJointInterface *effort_joint_interface,
            ros::NodeHandle &root_nh, ros::NodeHandle &controller_nh) override;

  void update(const ros::Time &time, const ros::Duration &period) override;

  void starting(const ros::Time& time) override;
  void stopping(const ros::Time& time) override;

 private:
  void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg);
  void computeWheelCommands(double vx, double vy, double wz);
  void computeOdometry(const ros::Time &time, const ros::Duration &period);
  
  hardware_interface::JointHandle front_left_pivot_joint_, front_right_pivot_joint_, back_left_pivot_joint_, back_right_pivot_joint_;
  hardware_interface::JointHandle front_left_wheel_joint_, front_right_wheel_joint_,
      back_left_wheel_joint_, back_right_wheel_joint_;
  
  ros::Subscriber cmd_vel_sub_;
  ros::Publisher odom_pub_;
  std::shared_ptr<tf::TransformBroadcaster> tf_broadcaster_;
  ros::Time last_cmd_vel_time_;
  ros::Time last_odom_time_;
  
  double wheel_track_;
  double wheel_base_;
  double wheel_radius_;
  double cmd_vel_timeout_;
  double max_wheel_speed_;
  bool debug_print_;
  bool publish_odom_;
  bool publish_tf_;
  std::string odom_frame_id_;
  std::string base_frame_id_;
  
  // 当前速度指令
  double vx_, vy_, wz_;
  std::mutex cmd_vel_mutex_;
  
  // 轮子命令
  double pivot_cmd_[4];
  double wheel_cmd_[4];
  
  // 上一次有效的转向角度（用于零速度时保持方向）
  double last_valid_pivot_cmd_[4];
  
  // 里程计相关变量
  double x_, y_, theta_;  // 位置和朝向
  double vx_actual_, vy_actual_, wz_actual_;  // 实际计算出的速度
  
  control_toolbox::Pid pid_lf_, pid_rf_, pid_lb_, pid_rb_;
  control_toolbox::Pid pid_lf_wheel_, pid_rf_wheel_, pid_lb_wheel_, pid_rb_wheel_;
};
}// namespace sentry_chassis_controller

#endif //SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H