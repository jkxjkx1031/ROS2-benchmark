#include <chrono>
#include <string>
#include <memory>
#include <functional>
#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

class TestSubscriber : public rclcpp::Node
{
public:
    TestSubscriber(const char *node_name = "test_subscriber"): Node(node_name)
    {
        sub_ = create_subscription<std_msgs::msg::String>(
            "topic", 1,
            std::bind(&TestSubscriber::topic_callback, this, std::placeholders::_1)
        );
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
    {
        // RCLCPP_INFO(get_logger(), "Get: %s", msg->data.c_str());
        int n = std::stoi(msg->data);
        std::cout << n << " " <<
            duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << "\n";
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    if (argc <= 1)
        rclcpp::spin(std::make_shared<TestSubscriber>());
    else
        rclcpp::spin(std::make_shared<TestSubscriber>(argv[1]));
    rclcpp::shutdown();
    return 0;
}