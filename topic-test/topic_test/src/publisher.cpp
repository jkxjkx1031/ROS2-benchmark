#include <chrono>
#include <string>
#include <memory>
#include <functional>
#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

class TestPublisher : public rclcpp::Node
{
public:
    TestPublisher(): Node("test_publisher")
    {
        pub_ = create_publisher<std_msgs::msg::String>("topic", 10);
        timer_ = create_wall_timer(
            500ms, std::bind(&TestPublisher::timer_callback, this)
        );
    }
private:
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int counter_ = 0;
    void timer_callback()
    {
        auto msg = std_msgs::msg::String();
        msg.data = std::to_string(++counter_);
        msg.data = std::string(10 - msg.data.size(), ' ') + msg.data;   // align messages
        // RCLCPP_INFO(get_logger(), "Put: %s", msg.data.c_str());
        std::cout << counter_ << " " <<
            duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() << "\n";
        pub_->publish(msg);
    }
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TestPublisher>());
    rclcpp::shutdown();
    return 0;
}