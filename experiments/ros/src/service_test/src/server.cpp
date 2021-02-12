#include "ros/ros.h"
#include "service_test/Echo.h"

bool echo(service_test::Echo::Request &req, service_test::Echo::Response &res)
{
    res.resStr = req.reqStr;
    return true;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "echo_server");
    ros::NodeHandle n;

    ros::ServiceServer service = n.advertiseService("echo", echo);
    ROS_INFO("Server ready.");
    ros::spin();

    return 0;
}