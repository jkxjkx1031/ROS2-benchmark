#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

class Stats
{
public:
    static std::atomic_int message_count;
    static int latency[20][20010];
    static void get_latency_stats(double &minLat, double &maxLat);
    static void reset()
    {
        message_count = 0;
        memset(latency, 0, sizeof(latency));
    }
};

std::atomic_int Stats::message_count;
int Stats::latency[20][20010];

void Stats::get_latency_stats(double &avgMin, double &avgMax)
{
    double avgi = 0, avga = 0;
    int n = 0;
    for (int i = 1; i <= 20000; i++)
    {
        int minLat = 0x3f3f3f3f, maxLat = 0;
        for (int j = 1; j <= 16; j++)
            if (latency[j][i] > 0)
                minLat = std::min(minLat, latency[j][i]),
                maxLat = std::max(maxLat, latency[j][i]);
        if (maxLat > 0)
        {
            n++;
            avgi += (minLat - avgi) / n;
            avga += (maxLat - avga) / n;
        }
    }
    avgMin = avgi, avgMax = avga;
}

class TestSubscriber : public rclcpp::Node
{
public:
    TestSubscriber(int id): Node(std::string("s") + std::to_string(id)), id_(id)
    {
        sub_ = create_subscription<std_msgs::msg::String>(
            "topic", 1,
            std::bind(&TestSubscriber::topic_callback, this, std::placeholders::_1)
        );
    }
private:
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
    int id_;
    void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
    {
        std::string content = msg->data;
        auto now = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        // std::cerr << "debug: " << content.substr(0, content.find(";")) << " " << content.substr(content.find(";") + 1) << "\n";
        auto n = std::stoi(content.substr(0, content.find(";")));
        auto ts = std::stoll(content.substr(content.find(";") + 1));
        Stats::latency[id_][n] = now - ts;
    }
};

void run_subscriber(int node_id, int duration)
{
    std::promise<void> p;
    rclcpp::spin_until_future_complete(
        std::make_shared<TestSubscriber>(node_id),
        std::shared_future<void>(p.get_future()),
        seconds(duration)
    );
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    const int DURATION = 300;
    const int MAX_NODE_COUNT = 1;
    std::vector<std::thread> SubscriberRunners(MAX_NODE_COUNT);
    for (int n = 1; n <= MAX_NODE_COUNT; n++)
    {
        Stats::reset();
        for (int i = 0; i < n; i++)
            SubscriberRunners[i] = std::thread(run_subscriber, i + 1, DURATION);
        for (int i = 0; i < n; i++)
            SubscriberRunners[i].join();
        // double avgMin, avgMax;
        // Stats::get_latency_stats(avgMin, avgMax);
        // std::cout << avgMin << " " << avgMax << "\n";
        for (int i = 1; i <= 10000; i++)
            if (Stats::latency[1][i] > 0)
                std::cout << Stats::latency[1][i] << "\n";
        // std::cout << Stats::message_count << "\n";
        std::cerr << "n=" << n << " completed.\n";
    }
    rclcpp::shutdown();
    return 0;
}