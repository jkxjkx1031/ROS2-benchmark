#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/trigger.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;

class Stats
{
public:
    static std::vector<long long> rtt[20];
    static void reset()
    {
        for (int i = 0; i < 20; i++)
            rtt[i].clear();
    }
};

std::vector<long long> Stats::rtt[20];

inline long long get_timestamp_now()
{
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

void run_client(int node_id, int duration)
{
    std::string node_name = std::string("c") + std::to_string(node_id);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared(node_name.c_str());
    rclcpp::Client<example_interfaces::srv::Trigger>::SharedPtr client = 
        node->create_client<example_interfaces::srv::Trigger>("test_service");

    while (!client->wait_for_service(1s)) /* wait */;

    const int MAX_REQUEST_COUNT = 10000;
    auto request = std::make_shared<example_interfaces::srv::Trigger::Request>();
    long long start = get_timestamp_now();
    for (int i = 1; i <= MAX_REQUEST_COUNT; i++)
    {
        long long send_ts = get_timestamp_now();
        auto result = client->async_send_request(request);
        rclcpp::spin_until_future_complete(node, result);
        long long receive_ts = get_timestamp_now();
        Stats::rtt[node_id].push_back(receive_ts - send_ts);
    }
}

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    const int DURATION = 1;
    const int MAX_NODE_COUNT = 1;
    std::vector<std::thread> ClientRunners(MAX_NODE_COUNT);
    for (int n = 1; n <= MAX_NODE_COUNT; n++)
    {
        Stats::reset();
        for (int i = 0; i < n; i++)
            ClientRunners[i] = std::thread(run_client, i + 1, DURATION);
        for (int i = 0; i < n; i++)
            ClientRunners[i].join();
        // double avg = 0;
        // int count = 0;
        // for (int i = 1; i <= n; i++)
        // {
        //     count += Stats::rtt[i].size();
        //     for (auto x : Stats::rtt[i])
        //         avg += x;
        // }
        // avg /= count;
        // std::cout << avg << "\n";
        for (auto x : Stats::rtt[1])
            std::cout << x << "\n";
        std::cerr << "n=" << n << " completed.\n";
    }
    rclcpp::shutdown();
    return 0;
}