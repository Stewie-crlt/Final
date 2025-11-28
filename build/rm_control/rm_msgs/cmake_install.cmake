# Install script for directory: /home/stewie/RosFinal/src/rm_control/rm_msgs

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/stewie/RosFinal/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/msg" TYPE FILE FILES
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/ActuatorState.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/BalanceState.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/BusState.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/DbusData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/Dart.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/ExchangerMsg.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/ChassisCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/ShootCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/ShootState.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/ShootBeforehandCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/GimbalCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/GimbalDesError.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/GimbalPosState.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/LegCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/LpData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/LocalHeatState.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/KalmanData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/MovingAverageData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/GpioData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/TofRadarData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/MultiDofCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/TagMsg.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/TagMsgArray.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/SentryDeviate.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/CustomControllerData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/VTKeyboardMouseData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/VTReceiverControlData.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/srv" TYPE FILE FILES
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/srv/ContinousDetectorSwitch.srv"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/srv/StatusChange.srv"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/srv/SetLimitVel.srv"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/srv/CameraStatus.srv"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/srv/EnableImuTrigger.srv"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/srv/EnableGyro.srv"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/msg/detection" TYPE FILE FILES
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/detection/PriorityArray.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/detection/TargetDetection.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/detection/TargetDetectionArray.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/detection/TrackData.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/msg/referee" TYPE FILE FILES
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/CapacityData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/GameRobotStatus.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/GameStatus.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/StateCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/EngineerUi.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/GameRobotHp.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/PowerHeatData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/EventData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/DartStatus.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/IcraBuffDebuffZoneStatus.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/SupplyProjectileAction.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/DartInfo.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RobotHurt.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/SentryAttackingTarget.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/ShootData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/BulletAllowance.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RfidStatus.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/DartClientCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/ManualToReferee.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RadarData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RadarMarkData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RobotsPositionData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/ClientMapSendData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/ClientMapReceiveData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/MapSentryData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/GameRobotPosData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/SentryInfo.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RadarInfo.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/Buff.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/RadarToSentry.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/SentryCmd.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/VisualizeStateData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/PowerManagementSampleAndStatusData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/PowerManagementInitializationExceptionData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/PowerManagementProcessStackOverflowData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/PowerManagementSystemExceptionData.msg"
    "/home/stewie/RosFinal/src/rm_control/rm_msgs/msg/referee/PowerManagementUnknownExceptionData.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/action" TYPE FILE FILES "/home/stewie/RosFinal/src/rm_control/rm_msgs/action/Engineer.action")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/msg" TYPE FILE FILES
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerAction.msg"
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerActionGoal.msg"
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerActionResult.msg"
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerActionFeedback.msg"
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerGoal.msg"
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerResult.msg"
    "/home/stewie/RosFinal/devel/share/rm_msgs/msg/EngineerFeedback.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/cmake" TYPE FILE FILES "/home/stewie/RosFinal/build/rm_control/rm_msgs/catkin_generated/installspace/rm_msgs-msg-paths.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/stewie/RosFinal/devel/include/rm_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/roseus/ros" TYPE DIRECTORY FILES "/home/stewie/RosFinal/devel/share/roseus/ros/rm_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/common-lisp/ros" TYPE DIRECTORY FILES "/home/stewie/RosFinal/devel/share/common-lisp/ros/rm_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gennodejs/ros" TYPE DIRECTORY FILES "/home/stewie/RosFinal/devel/share/gennodejs/ros/rm_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND "/usr/bin/python3" -m compileall "/home/stewie/RosFinal/devel/lib/python3/dist-packages/rm_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3/dist-packages" TYPE DIRECTORY FILES "/home/stewie/RosFinal/devel/lib/python3/dist-packages/rm_msgs")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/stewie/RosFinal/build/rm_control/rm_msgs/catkin_generated/installspace/rm_msgs.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/cmake" TYPE FILE FILES "/home/stewie/RosFinal/build/rm_control/rm_msgs/catkin_generated/installspace/rm_msgs-msg-extras.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs/cmake" TYPE FILE FILES
    "/home/stewie/RosFinal/build/rm_control/rm_msgs/catkin_generated/installspace/rm_msgsConfig.cmake"
    "/home/stewie/RosFinal/build/rm_control/rm_msgs/catkin_generated/installspace/rm_msgsConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/rm_msgs" TYPE FILE FILES "/home/stewie/RosFinal/src/rm_control/rm_msgs/package.xml")
endif()

