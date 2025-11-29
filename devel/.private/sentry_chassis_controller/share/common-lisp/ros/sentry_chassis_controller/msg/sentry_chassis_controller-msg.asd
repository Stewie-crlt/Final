
(cl:in-package :asdf)

(defsystem "sentry_chassis_controller-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :std_msgs-msg
)
  :components ((:file "_package")
    (:file "ChassisState" :depends-on ("_package_ChassisState"))
    (:file "_package_ChassisState" :depends-on ("_package"))
  ))