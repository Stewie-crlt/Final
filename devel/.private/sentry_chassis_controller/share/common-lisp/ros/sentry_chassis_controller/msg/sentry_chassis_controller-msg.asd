
(cl:in-package :asdf)

(defsystem "sentry_chassis_controller-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "WheelState" :depends-on ("_package_WheelState"))
    (:file "_package_WheelState" :depends-on ("_package"))
  ))