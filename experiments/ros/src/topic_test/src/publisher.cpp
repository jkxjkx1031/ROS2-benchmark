#include <chrono>
#include <string>

#include "ros/ros.h"
#include "std_msgs/String.h"

using namespace std::chrono;

int main(int argc, char **argv)
{
    int samples = std::stoi(argv[1]);
    ros::init(argc, argv, "publisher");
    ros::NodeHandle n;
    ros::Publisher pub = n.advertise<std_msgs::String>("topic", 1000);
    ros::Rate loop_rate(50);
    int count = 0;
    while (ros::ok() && count <= samples)
    {
        std_msgs::String msg;
        if (count < samples)
            msg.data = std::to_string(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count());
        else
            msg.data = std::to_string(-1);
        // ROS_INFO("%s", msg.data.c_str());
        pub.publish(msg);
        count++;
        loop_rate.sleep();
    }
    return 0;
}