#include <chrono>
#include <string>
#include <iostream>

#include "ros/ros.h"
#include "std_msgs/String.h"

using namespace std::chrono;

double avg;
int samples;

void onReceive(const std_msgs::String::ConstPtr& msg)
{
    // ROS_INFO("I heard: [%s]", msg->data.c_str());
    long long trecv = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
    long long tsend = std::stoll(msg->data);
    if (tsend < 0)
    {
        std::cout << avg << " " << samples << "\n";
        ros::shutdown();
    }
    avg += (trecv - tsend - avg) / ++samples;
}

int main(int argc, char **argv)
{
    std::string name = std::string("subscriber_") + argv[1];
    ros::init(argc, argv, name.c_str());
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("topic", 1000, onReceive);
    ros::spin();
    return 0;
}