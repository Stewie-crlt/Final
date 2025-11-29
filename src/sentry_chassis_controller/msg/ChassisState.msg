# 底盘状态消息
Header header

# 指令速度
float64 cmd_vx
float64 cmd_vy
float64 cmd_wz

# 轮子期望转速 (rad/s)
float64[4] desired_wheel_speeds
# 轮子实际转速 (rad/s)
float64[4] actual_wheel_speeds

# 轮子期望转向角度 (rad)
float64[4] desired_steering_angles
# 轮子实际转向角度 (rad)
float64[4] actual_steering_angles

# 轮子控制力矩
float64[4] wheel_efforts
# 转向控制力矩
float64[4] steering_efforts

# 是否接收到有效指令
bool cmd_vel_active