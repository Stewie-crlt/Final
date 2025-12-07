cmake_minimum_required(VERSION 3.10)
project(sentry_chassis_controller)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Wall -Werror)

find_package(catkin REQUIRED
    COMPONENTS
    roscpp
    dynamic_reconfigure
    roslint
    controller_interface
    hardware_interface
    forward_command_controller
    pluginlib
    control_toolbox
    std_msgs
    message_generation
    geometry_msgs
    nav_msgs
    tf
)

find_package(Eigen3 REQUIRED)

# 添加消息生成
add_message_files(
  FILES
  ChassisState.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
    INCLUDE_DIRS
    include
    LIBRARIES
    ${PROJECT_NAME}
    CATKIN_DEPENDS
    roscpp
    dynamic_reconfigure
    roslint
    controller_interface
    hardware_interface
    forward_command_controller
    pluginlib
    control_toolbox
    std_msgs
    message_runtime
    geometry_msgs
    nav_msgs
    tf
    DEPENDS
    Eigen3
)

include_directories(
    include
    ${catkin_INCLUDE_DIRS}
    ${Eigen3_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
    src/sentry_chassis_controller.cpp
)



target_link_libraries(${PROJECT_NAME}
    ${catkin_LIBRARIES}
    Eigen3::Eigen
)

# 添加 teleop_keyboard 可执行文件
add_executable(teleop_keyboard src/teleop_keyboard.cpp)
add_dependencies(teleop_keyboard ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(teleop_keyboard ${catkin_LIBRARIES})

install(TARGETS teleop_keyboard
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)


# 安装库文件
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

# 安装头文件
install(DIRECTORY include/${PROJECT_NAME}/
    DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

# 安装配置文件
install(FILES config/controllers.yaml
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/config
)

# 安装插件文件
install(FILES sentry_chassis_controller_plugins.xml
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

roslint_cpp()