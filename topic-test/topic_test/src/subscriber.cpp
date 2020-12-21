#include <string>
#include <memory>
#include <functional>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;

class TestSubscriber : public rclcpp::Node
{
public:
    TestSubscriber(): Node("test_subscriber")
    {
        sub_ = create_subscription<std_msgs::msg::String>(
            "topic", 50,
            std::bind(&TestSubscriber::topic_callback, this, std::placeholders::_1)
        );
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
    {
        RCLCPP_INFO(get_logger(), "Get: %s", msg->data.c_str());
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TestSubscriber>());
    rclcpp::shutdown();
    return 0;
}