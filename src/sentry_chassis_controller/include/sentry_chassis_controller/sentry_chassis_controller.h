#ifndef SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H
#define SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H

#include <ros/ros.h>
#include <controller_interface/controller.h>
#include <hardware_interface/joint_command_interface.h>
#include <control_toolbox/pid.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Accel.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Bool.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <mutex>
#include <queue>
#include <dynamic_reconfigure/server.h>
#include "sentry_chassis_controller/PIDConfig.h"

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
  void computeWheelCommandsForSelfLock();
  void computeOdometry(const ros::Time &time, const ros::Duration &period);
  void computeAndPublishAcceleration(const ros::Time &time, const ros::Duration &period);
  void computeAndPublishPower(const ros::Time &time, const ros::Duration &period);
  void applyPowerLimits(double &torque_fl, double &torque_fr, 
                        double &torque_bl, double &torque_br, 
                        const double &vel_fl, const double &vel_fr,
                        const double &vel_bl, const double &vel_br);
  bool transformVelocityToBaseFrame(const geometry_msgs::Twist& world_vel, 
                                   geometry_msgs::Twist& base_vel);
  
  void reconfigureCallback(sentry_chassis_controller::PIDConfig &config, uint32_t level);
  
  void updateSelfLockState(const ros::Time &time, const ros::Duration &period);
  void enterSelfLockMode();
  void exitSelfLockMode();
  
  hardware_interface::JointHandle front_left_pivot_joint_, front_right_pivot_joint_, back_left_pivot_joint_, back_right_pivot_joint_;
  hardware_interface::JointHandle front_left_wheel_joint_, front_right_wheel_joint_,
      back_left_wheel_joint_, back_right_wheel_joint_;
  
  ros::Subscriber cmd_vel_sub_;
  ros::Publisher odom_pub_;
  ros::Publisher accel_pub_;
  ros::Publisher total_power_pub_;      // 总功率发布器
  ros::Publisher consumption_power_pub_; // 消耗功率发布器
  ros::Publisher regeneration_power_pub_; // 再生功率发布器
  ros::Publisher power_limit_pub_;       // 功率限制发布器
  ros::Publisher power_limited_pub_;     // 功率限制状态发布器
  
  std::shared_ptr<tf::TransformBroadcaster> tf_broadcaster_;
  std::unique_ptr<tf::TransformListener> tf_listener_;
  
  ros::Time last_cmd_vel_time_;
  ros::Time last_odom_time_;
  ros::Time last_accel_time_;
  ros::Time last_power_time_;
  
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
  
  bool world_vel_mode_;
  std::string world_frame_id_;
  
  // 功率控制参数（从配置文件读取，不支持动态调参）
  double max_total_power_;
  bool power_limit_enabled_;
  double current_power_limit_;
  double power_filter_time_constant_;
  
  // 功率计算相关
  double total_power_;
  double power_consumption_;
  double power_regeneration_;
  bool power_limited_;
  std::queue<double> power_history_;
  const size_t POWER_HISTORY_SIZE = 10;
  
  // 加速度限制参数
  double max_linear_acceleration_;
  double max_angular_acceleration_;
  double last_vx_, last_vy_, last_wz_;
  
  bool enable_self_lock_;
  double self_lock_timeout_;
  double self_lock_angle_;
  bool in_self_lock_mode_;
  ros::Time self_lock_start_time_;
  
  boost::shared_ptr<dynamic_reconfigure::Server<sentry_chassis_controller::PIDConfig>> dyn_reconfig_server_;
  dynamic_reconfigure::Server<sentry_chassis_controller::PIDConfig>::CallbackType dyn_reconfig_callback_;
  std::mutex reconfigure_mutex_;
  
  double vx_, vy_, wz_;
  std::mutex cmd_vel_mutex_;
  std::mutex power_mutex_;
  
  double pivot_cmd_[4];
  double wheel_cmd_[4];
  
  double last_valid_pivot_cmd_[4];
  
  double x_, y_, theta_;
  double vx_actual_, vy_actual_, wz_actual_;
  
  double ax_actual_, ay_actual_, awz_actual_;
  
  control_toolbox::Pid pid_lf_, pid_rf_, pid_lb_, pid_rb_;
  control_toolbox::Pid pid_lf_wheel_, pid_rf_wheel_, pid_lb_wheel_, pid_rb_wheel_;
};
}// namespace sentry_chassis_controller

#endif //SENTRY_CHASSIS_CONTROLLER_SENTRY_CHASSIS_CONTROLLER_H