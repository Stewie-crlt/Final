#ifndef SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H
#define SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H

#include <ros/ros.h>
#include <controller_interface/controller.h>
#include <hardware_interface/joint_command_interface.h>
#include <control_toolbox/pid.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float32.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <mutex>
#include <memory>
#include <dynamic_reconfigure/server.h>
#include <Eigen/Dense>
#include <sentry_chassis_controller/PIDConfig.h>

namespace sentry_chassis_controller {

class SentryChassisController : public controller_interface::Controller<hardware_interface::EffortJointInterface> {
 public:
  SentryChassisController();
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
  bool transformVelocityToBaseFrame(const geometry_msgs::Twist& world_vel, 
                                   geometry_msgs::Twist& base_vel);
  void setLockMode(bool enable);
  
  // 动态重配置回调函数
  void reconfigureCallback(sentry_chassis_controller::PIDConfig &config, uint32_t level);
  
  hardware_interface::JointHandle front_left_pivot_joint_, front_right_pivot_joint_, 
                                  back_left_pivot_joint_, back_right_pivot_joint_;
  hardware_interface::JointHandle front_left_wheel_joint_, front_right_wheel_joint_,
      back_left_wheel_joint_, back_right_wheel_joint_;
  
  ros::Subscriber cmd_vel_sub_;
  ros::Publisher odom_pub_;
  ros::Publisher distance_pub_;
  std::shared_ptr<tf::TransformBroadcaster> tf_broadcaster_;
  std::unique_ptr<tf::TransformListener> tf_listener_;
  
  // 动态重配置服务器
  std::shared_ptr<dynamic_reconfigure::Server<sentry_chassis_controller::PIDConfig>> dyn_reconf_server_;
  
  ros::Time last_cmd_vel_time_;
  ros::Time last_odom_time_;
  
  double wheel_track_;
  double wheel_base_;
  double wheel_radius_;
  double cmd_vel_timeout_;
  double max_wheel_speed_;
  double vx_, vy_, wz_;
  std::mutex cmd_vel_mutex_;
  
  double pivot_cmd_[4];
  double wheel_cmd_[4];
  
  double last_valid_pivot_cmd_[4];
  
  double x_, y_, theta_;
  double vx_actual_, vy_actual_, wz_actual_;
  double total_distance_;
  
  bool debug_print_;
  bool publish_odom_;
  bool publish_tf_;
  std::string odom_frame_id_;
  std::string base_frame_id_;
  
  bool world_vel_mode_;
  std::string world_frame_id_;
  
  bool enable_lock_mode_;
  double lock_timeout_;
  double lock_angle_;  
  bool is_locked_;
  
  control_toolbox::Pid pid_lf_, pid_rf_, pid_lb_, pid_rb_;
  control_toolbox::Pid pid_lf_wheel_, pid_rf_wheel_, pid_lb_wheel_, pid_rb_wheel_;
};

} // namespace sentry_chassis_controller

#endif // SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H