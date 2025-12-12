# 一.项目地址:
## git clone https://github.com/Stewie-crlt/Final.git



# 二. 项目依赖
## 本项目基于ubuntu20.04系统，开发者需安装ros-noetic，并在vscode编译器下运行。


# 三. 如何运行项目
## 开发者进入本项目后，进入/src/sentry_chassis_controller目录可看到本项目，或者在/src目录下打开终端使用catkin build编译，后使用roslaunch sentry_chassis_controller sentry_simulation.launch运行项目
















# 特色功能

## 计算小车行走总路程（已删除）

# 存在问题
## 1.调试pid时，想要写个配置文件，可是配置文件暂时无法根据所想的生成.h文件(已解决)
## 2.写键盘控制器代码时每次输入都要按回车键
### 解决方式：读取终端设置