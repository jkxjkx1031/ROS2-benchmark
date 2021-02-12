#include "ros/ros.h"
#include "service_test/Echo.h"

#include <chrono>
#include <string>
#include <iostream>

using namespace std::chrono;

int main(int argc, char **argv)
{
    std::string name = std::string("client_") + argv[1];
    int samples = std::stoi(argv[2]);
    ros::init(argc, argv, name.c_str());

    ros::NodeHandle n;
    ros::ServiceClient client = n.serviceClient<service_test::Echo>("echo");
    service_test::Echo srv;
    srv.request.reqStr = std::string("_hereis10B");

    double avg = 0;
    for (int i = 1; i <= samples; i++)
    {
        long long start = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        client.call(srv);
        long long end = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        avg += (end - start - avg) / i;
    }
    std::cout << avg << "\n";

    return 0;
}