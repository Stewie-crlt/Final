# generated from catkin/cmake/template/pkg.context.pc.in
CATKIN_PACKAGE_PREFIX = ""
PROJECT_PKG_CONFIG_INCLUDE_DIRS = "${prefix}/include".split(';') if "${prefix}/include" != "" else []
PROJECT_CATKIN_DEPENDS = "roscpp;roslint;controller_interface;hardware_interface;forward_command_controller;pluginlib;control_toolbox;std_msgs;message_runtime;geometry_msgs;nav_msgs;tf".replace(';', ' ')
PKG_CONFIG_LIBRARIES_WITH_PREFIX = "-lsentry_chassis_controller".split(';') if "-lsentry_chassis_controller" != "" else []
PROJECT_NAME = "sentry_chassis_controller"
PROJECT_SPACE_DIR = "/home/stewie/RosFinal/install"
PROJECT_VERSION = "0.0.0"
