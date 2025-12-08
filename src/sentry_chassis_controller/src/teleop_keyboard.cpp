#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/select.h>


#define KEYCODE_SPACE 0x20  // Space key
#define KEYCODE_ESC 0x1B    // ESC key

class TeleopKeyboard
{
public:
    TeleopKeyboard();
    void keyLoop();

private:
    ros::NodeHandle nh_;
    ros::Publisher twist_pub_;
    double linear_vel_, angular_vel_, lateral_vel_;
    double linear_vel_step_, angular_vel_step_, lateral_vel_step_;
    geometry_msgs::Twist twist_;
};

//初始化
TeleopKeyboard::TeleopKeyboard():
    linear_vel_(0.0),
    angular_vel_(0.0),
    lateral_vel_(0.0),
    linear_vel_step_(0.3),
    angular_vel_step_(1.0),
    lateral_vel_step_(0.3)

{
    // 初始化参数
    nh_.param("linear_vel_step", linear_vel_step_, linear_vel_step_);
    nh_.param("angular_vel_step", angular_vel_step_, angular_vel_step_);
    nh_.param("lateral_vel_step", lateral_vel_step_, lateral_vel_step_);
    
    twist_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    
    ROS_INFO("Teleop Keyboard Control Started");
    ROS_INFO("--------------------------------");
    ROS_INFO("  Linear (X): %.2f m/s", linear_vel_step_);
    ROS_INFO("  Angular (Z): %.2f rad/s", angular_vel_step_);
    ROS_INFO("  Lateral (Y): %.2f m/s", lateral_vel_step_);
    ROS_INFO("--------------------------------");
}

int kfd = 0;
struct termios cooked, raw;

void quit(int sig)
{
    (void)sig;
    tcsetattr(kfd, TCSANOW, &cooked);
    ros::shutdown();
    exit(0);
}

void TeleopKeyboard::keyLoop()
{
    char c;
    bool dirty = false;
    
    // 获取终端设置
    tcgetattr(kfd, &cooked);//获取当前终端属性
    memcpy(&raw, &cooked, sizeof(struct termios));
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    tcsetattr(kfd, TCSANOW, &raw);
    
    puts("Reading from keyboard");
    puts("Press ESC to quit");
    
    struct timeval tv;
    fd_set readfds;
    
    while (ros::ok())
    {
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms timeout
        
        FD_ZERO(&readfds);
        FD_SET(kfd, &readfds);
        
        int select_ret = select(kfd + 1, &readfds, NULL, NULL, &tv);
        
        if (select_ret == -1)
        {
            perror("select():");
            exit(-1);
        }
        else if (select_ret == 0)
        {
            // 超时，没有按键输入，继续发布当前速度
            if (dirty)
            {
                twist_.linear.x = linear_vel_;
                twist_.linear.y = lateral_vel_;
                twist_.angular.z = angular_vel_;
                twist_pub_.publish(twist_);
                dirty = false;
            }
            continue;
        }
        
        // 有按键输入
        if (read(kfd, &c, 1) < 0)
        {
            perror("read():");
            exit(-1);
        }
        
        // 检查是否是ESC键
        if (c == KEYCODE_ESC)
        {
            ROS_INFO("ESC pressed, shutting down...");
            break;
        }
        
        dirty = true;
        
        switch(c)
        {
        case 'w':  // 设置前进速度
            linear_vel_ = linear_vel_step_;
            lateral_vel_ = 0.0;
            angular_vel_ = 0.0;
            ROS_INFO("FORWARD: linear speed = %.2f m/s", linear_vel_);
            break;
        case 's':  // 设置后退速度
            linear_vel_ = -linear_vel_step_;
            lateral_vel_ = 0.0;
            angular_vel_ = 0.0;
            ROS_INFO("BACKWARD: linear speed = %.2f m/s", linear_vel_);
            break;
        case 'a':  // 设置左转速度
            angular_vel_ = angular_vel_step_;
            linear_vel_ = 0.0;
            lateral_vel_ = 0.0;
            ROS_INFO("LEFT TURN: angular speed = %.2f rad/s", angular_vel_);
            break;
        case 'd':  // 设置右转速度
            angular_vel_ = -angular_vel_step_;
            linear_vel_ = 0.0;
            lateral_vel_ = 0.0;
            ROS_INFO("RIGHT TURN: angular speed = %.2f rad/s", angular_vel_);
            break;
        case 'q':  // 设置左移速度
            lateral_vel_ = lateral_vel_step_;
            linear_vel_ = 0.0;
            angular_vel_ = 0.0;
            ROS_INFO("LEFT SIDEWAYS: lateral speed = %.2f m/s", lateral_vel_);
            break;
        case 'e':  // 设置右移速度
            lateral_vel_ = -lateral_vel_step_;
            linear_vel_ = 0.0;
            angular_vel_ = 0.0;
            ROS_INFO("RIGHT SIDEWAYS: lateral speed = %.2f m/s", lateral_vel_);
            break;
        case KEYCODE_SPACE:  // 空格键 - 停止
            linear_vel_ = 0.0;
            angular_vel_ = 0.0;
            lateral_vel_ = 0.0;
            ROS_INFO("STOPPING all motion");
            break;
        case 'z':  // 减小速度
            linear_vel_step_ *= 0.8;
            angular_vel_step_ *= 0.8;
            lateral_vel_step_ *= 0.8;
            if (linear_vel_step_ < 0.01) linear_vel_step_ = 0.01;
            if (angular_vel_step_ < 0.01) angular_vel_step_ = 0.01;
            if (lateral_vel_step_ < 0.01) lateral_vel_step_ = 0.01;
            ROS_INFO("Speed steps DECREASED: linear=%.3f, angular=%.3f, lateral=%.3f", 
                     linear_vel_step_, angular_vel_step_, lateral_vel_step_);
            break;
        case 'x':  // 增加速度
            linear_vel_step_ *= 1.2;
            angular_vel_step_ *= 1.2;
            lateral_vel_step_ *= 1.2;
            ROS_INFO("Speed steps INCREASED: linear=%.3f, angular=%.3f, lateral=%.3f", 
                     linear_vel_step_, angular_vel_step_, lateral_vel_step_);
            break;
        case 'c':  // 重置速度
            linear_vel_step_ = 0.3;
            angular_vel_step_ = 1.0;
            lateral_vel_step_ = 0.3;
            ROS_INFO("Speed steps RESET to default values");
            break;
        default:
            dirty = false;
            break;
        }
        
        // 发布速度指令
        if (dirty)
        {
            twist_.linear.x = linear_vel_;
            twist_.linear.y = lateral_vel_;
            twist_.linear.z = 0.0;
            twist_.angular.x = 0.0;
            twist_.angular.y = 0.0;
            twist_.angular.z = angular_vel_;
            
            twist_pub_.publish(twist_);
        }
    }
    
    // 退出时发送停止指令
    twist_.linear.x = 0.0;
    twist_.linear.y = 0.0;
    twist_.angular.z = 0.0;
    twist_pub_.publish(twist_);
    
    // 恢复终端设置
    tcsetattr(kfd, TCSANOW, &cooked);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "teleop_keyboard");
    TeleopKeyboard teleop_keyboard;
    
    signal(SIGINT, quit);
    
    teleop_keyboard.keyLoop();
    
    return 0;
}