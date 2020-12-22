#include <chrono>
#include <memory>
#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/trigger.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

inline long long get_timestamp_now()
{
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);

    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("client_node");
    rclcpp::Client<example_interfaces::srv::Trigger>::SharedPtr client = 
        node->create_client<example_interfaces::srv::Trigger>("test_service");

    while (!client->wait_for_service(1s))
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Waiting for service...");
    auto request = std::make_shared<example_interfaces::srv::Trigger::Request>();
    for (int i = 1; i <= 10000; i++)
    {
        long long start = get_timestamp_now();
        auto result = client->async_send_request(request);
        rclcpp::spin_until_future_complete(node, result);
        std::cout << get_timestamp_now() - start << "\n";
        if (i % 500 == 0)
            std::cerr << i / 100 << "% completed...\n";
    }

    rclcpp::shutdown();

    return 0;
}