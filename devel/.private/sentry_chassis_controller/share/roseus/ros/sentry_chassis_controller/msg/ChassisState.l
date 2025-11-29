;; Auto-generated. Do not edit!


(when (boundp 'sentry_chassis_controller::ChassisState)
  (if (not (find-package "SENTRY_CHASSIS_CONTROLLER"))
    (make-package "SENTRY_CHASSIS_CONTROLLER"))
  (shadow 'ChassisState (find-package "SENTRY_CHASSIS_CONTROLLER")))
(unless (find-package "SENTRY_CHASSIS_CONTROLLER::CHASSISSTATE")
  (make-package "SENTRY_CHASSIS_CONTROLLER::CHASSISSTATE"))

(in-package "ROS")
;;//! \htmlinclude ChassisState.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass sentry_chassis_controller::ChassisState
  :super ros::object
  :slots (_header _cmd_vx _cmd_vy _cmd_wz _desired_wheel_speeds _actual_wheel_speeds _desired_steering_angles _actual_steering_angles _wheel_efforts _steering_efforts _cmd_vel_active ))

(defmethod sentry_chassis_controller::ChassisState
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:cmd_vx __cmd_vx) 0.0)
    ((:cmd_vy __cmd_vy) 0.0)
    ((:cmd_wz __cmd_wz) 0.0)
    ((:desired_wheel_speeds __desired_wheel_speeds) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:actual_wheel_speeds __actual_wheel_speeds) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:desired_steering_angles __desired_steering_angles) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:actual_steering_angles __actual_steering_angles) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:wheel_efforts __wheel_efforts) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:steering_efforts __steering_efforts) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:cmd_vel_active __cmd_vel_active) nil)
    )
   (send-super :init)
   (setq _header __header)
   (setq _cmd_vx (float __cmd_vx))
   (setq _cmd_vy (float __cmd_vy))
   (setq _cmd_wz (float __cmd_wz))
   (setq _desired_wheel_speeds __desired_wheel_speeds)
   (setq _actual_wheel_speeds __actual_wheel_speeds)
   (setq _desired_steering_angles __desired_steering_angles)
   (setq _actual_steering_angles __actual_steering_angles)
   (setq _wheel_efforts __wheel_efforts)
   (setq _steering_efforts __steering_efforts)
   (setq _cmd_vel_active __cmd_vel_active)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:cmd_vx
   (&optional __cmd_vx)
   (if __cmd_vx (setq _cmd_vx __cmd_vx)) _cmd_vx)
  (:cmd_vy
   (&optional __cmd_vy)
   (if __cmd_vy (setq _cmd_vy __cmd_vy)) _cmd_vy)
  (:cmd_wz
   (&optional __cmd_wz)
   (if __cmd_wz (setq _cmd_wz __cmd_wz)) _cmd_wz)
  (:desired_wheel_speeds
   (&optional __desired_wheel_speeds)
   (if __desired_wheel_speeds (setq _desired_wheel_speeds __desired_wheel_speeds)) _desired_wheel_speeds)
  (:actual_wheel_speeds
   (&optional __actual_wheel_speeds)
   (if __actual_wheel_speeds (setq _actual_wheel_speeds __actual_wheel_speeds)) _actual_wheel_speeds)
  (:desired_steering_angles
   (&optional __desired_steering_angles)
   (if __desired_steering_angles (setq _desired_steering_angles __desired_steering_angles)) _desired_steering_angles)
  (:actual_steering_angles
   (&optional __actual_steering_angles)
   (if __actual_steering_angles (setq _actual_steering_angles __actual_steering_angles)) _actual_steering_angles)
  (:wheel_efforts
   (&optional __wheel_efforts)
   (if __wheel_efforts (setq _wheel_efforts __wheel_efforts)) _wheel_efforts)
  (:steering_efforts
   (&optional __steering_efforts)
   (if __steering_efforts (setq _steering_efforts __steering_efforts)) _steering_efforts)
  (:cmd_vel_active
   (&optional (__cmd_vel_active :null))
   (if (not (eq __cmd_vel_active :null)) (setq _cmd_vel_active __cmd_vel_active)) _cmd_vel_active)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float64 _cmd_vx
    8
    ;; float64 _cmd_vy
    8
    ;; float64 _cmd_wz
    8
    ;; float64[4] _desired_wheel_speeds
    (* 8    4)
    ;; float64[4] _actual_wheel_speeds
    (* 8    4)
    ;; float64[4] _desired_steering_angles
    (* 8    4)
    ;; float64[4] _actual_steering_angles
    (* 8    4)
    ;; float64[4] _wheel_efforts
    (* 8    4)
    ;; float64[4] _steering_efforts
    (* 8    4)
    ;; bool _cmd_vel_active
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; float64 _cmd_vx
       (sys::poke _cmd_vx (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _cmd_vy
       (sys::poke _cmd_vy (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _cmd_wz
       (sys::poke _cmd_wz (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64[4] _desired_wheel_speeds
     (dotimes (i 4)
       (sys::poke (elt _desired_wheel_speeds i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _actual_wheel_speeds
     (dotimes (i 4)
       (sys::poke (elt _actual_wheel_speeds i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _desired_steering_angles
     (dotimes (i 4)
       (sys::poke (elt _desired_steering_angles i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _actual_steering_angles
     (dotimes (i 4)
       (sys::poke (elt _actual_steering_angles i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _wheel_efforts
     (dotimes (i 4)
       (sys::poke (elt _wheel_efforts i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _steering_efforts
     (dotimes (i 4)
       (sys::poke (elt _steering_efforts i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; bool _cmd_vel_active
       (if _cmd_vel_active (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float64 _cmd_vx
     (setq _cmd_vx (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _cmd_vy
     (setq _cmd_vy (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _cmd_wz
     (setq _cmd_wz (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64[4] _desired_wheel_speeds
   (dotimes (i (length _desired_wheel_speeds))
     (setf (elt _desired_wheel_speeds i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _actual_wheel_speeds
   (dotimes (i (length _actual_wheel_speeds))
     (setf (elt _actual_wheel_speeds i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _desired_steering_angles
   (dotimes (i (length _desired_steering_angles))
     (setf (elt _desired_steering_angles i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _actual_steering_angles
   (dotimes (i (length _actual_steering_angles))
     (setf (elt _actual_steering_angles i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _wheel_efforts
   (dotimes (i (length _wheel_efforts))
     (setf (elt _wheel_efforts i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _steering_efforts
   (dotimes (i (length _steering_efforts))
     (setf (elt _steering_efforts i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; bool _cmd_vel_active
     (setq _cmd_vel_active (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(setf (get sentry_chassis_controller::ChassisState :md5sum-) "be09b6eddd3e724282cde2412802ba78")
(setf (get sentry_chassis_controller::ChassisState :datatype-) "sentry_chassis_controller/ChassisState")
(setf (get sentry_chassis_controller::ChassisState :definition-)
      "# 底盘状态消息
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
================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
string frame_id

")



(provide :sentry_chassis_controller/ChassisState "be09b6eddd3e724282cde2412802ba78")


