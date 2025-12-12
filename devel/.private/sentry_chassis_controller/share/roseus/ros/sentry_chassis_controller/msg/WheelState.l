;; Auto-generated. Do not edit!


(when (boundp 'sentry_chassis_controller::WheelState)
  (if (not (find-package "SENTRY_CHASSIS_CONTROLLER"))
    (make-package "SENTRY_CHASSIS_CONTROLLER"))
  (shadow 'WheelState (find-package "SENTRY_CHASSIS_CONTROLLER")))
(unless (find-package "SENTRY_CHASSIS_CONTROLLER::WHEELSTATE")
  (make-package "SENTRY_CHASSIS_CONTROLLER::WHEELSTATE"))

(in-package "ROS")
;;//! \htmlinclude WheelState.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass sentry_chassis_controller::WheelState
  :super ros::object
  :slots (_header _steering_angle _wheel_speed _wheel_velocity _wheel_torque _wheel_current ))

(defmethod sentry_chassis_controller::WheelState
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:steering_angle __steering_angle) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:wheel_speed __wheel_speed) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:wheel_velocity __wheel_velocity) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:wheel_torque __wheel_torque) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:wheel_current __wheel_current) (make-array 4 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _header __header)
   (setq _steering_angle __steering_angle)
   (setq _wheel_speed __wheel_speed)
   (setq _wheel_velocity __wheel_velocity)
   (setq _wheel_torque __wheel_torque)
   (setq _wheel_current __wheel_current)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:steering_angle
   (&optional __steering_angle)
   (if __steering_angle (setq _steering_angle __steering_angle)) _steering_angle)
  (:wheel_speed
   (&optional __wheel_speed)
   (if __wheel_speed (setq _wheel_speed __wheel_speed)) _wheel_speed)
  (:wheel_velocity
   (&optional __wheel_velocity)
   (if __wheel_velocity (setq _wheel_velocity __wheel_velocity)) _wheel_velocity)
  (:wheel_torque
   (&optional __wheel_torque)
   (if __wheel_torque (setq _wheel_torque __wheel_torque)) _wheel_torque)
  (:wheel_current
   (&optional __wheel_current)
   (if __wheel_current (setq _wheel_current __wheel_current)) _wheel_current)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float64[4] _steering_angle
    (* 8    4)
    ;; float64[4] _wheel_speed
    (* 8    4)
    ;; float64[4] _wheel_velocity
    (* 8    4)
    ;; float64[4] _wheel_torque
    (* 8    4)
    ;; float64[4] _wheel_current
    (* 8    4)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; float64[4] _steering_angle
     (dotimes (i 4)
       (sys::poke (elt _steering_angle i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _wheel_speed
     (dotimes (i 4)
       (sys::poke (elt _wheel_speed i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _wheel_velocity
     (dotimes (i 4)
       (sys::poke (elt _wheel_velocity i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _wheel_torque
     (dotimes (i 4)
       (sys::poke (elt _wheel_torque i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;; float64[4] _wheel_current
     (dotimes (i 4)
       (sys::poke (elt _wheel_current i) (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float64[4] _steering_angle
   (dotimes (i (length _steering_angle))
     (setf (elt _steering_angle i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _wheel_speed
   (dotimes (i (length _wheel_speed))
     (setf (elt _wheel_speed i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _wheel_velocity
   (dotimes (i (length _wheel_velocity))
     (setf (elt _wheel_velocity i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _wheel_torque
   (dotimes (i (length _wheel_torque))
     (setf (elt _wheel_torque i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;; float64[4] _wheel_current
   (dotimes (i (length _wheel_current))
     (setf (elt _wheel_current i) (sys::peek buf ptr- :double)) (incf ptr- 8)
     )
   ;;
   self)
  )

(setf (get sentry_chassis_controller::WheelState :md5sum-) "1968715a3f1131762c929bdab65e2993")
(setf (get sentry_chassis_controller::WheelState :datatype-) "sentry_chassis_controller/WheelState")
(setf (get sentry_chassis_controller::WheelState :definition-)
      "
std_msgs/Header header
float64[4] steering_angle   # 四个轮子的转向角度（弧度）
float64[4] wheel_speed      # 四个轮子的转速(rad/s)
float64[4] wheel_velocity   # 四个轮子的线速度(m/s)
float64[4] wheel_torque     # 四个轮子的输出扭矩(Nm)
float64[4] wheel_current    # 四个轮子的电流(A)

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



(provide :sentry_chassis_controller/WheelState "1968715a3f1131762c929bdab65e2993")


