#include "SimpleTopicPubSubTypes.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/DataReaderListener.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>

#include <thread>

using namespace eprosima::fastdds::dds;
using namespace std::chrono;

class SimpleSubscriber
{
private:

    DomainParticipant* participant_;

    Subscriber* subscriber_;

    DataReader* reader_;

    Topic* topic_;

    TypeSupport type_;

    class SubListener : public DataReaderListener
    {
    public:

        SubListener()
            : latency(0), samples(0)
        {
        }

        ~SubListener() override
        {
        }

        void on_subscription_matched(
                DataReader*,
                const SubscriptionMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                std::cerr << "Subscriber matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                std::cerr << "Subscriber unmatched." << std::endl;
            }
            else
            {
                std::cerr << info.current_count_change
                        << " is not a valid value for SubscriptionMatchedStatus current count change" << std::endl;
            }
        }

        void on_data_available(
                DataReader* reader) override
        {
            SampleInfo info;
            if (reader->take_next_sample(&content_, &info) == ReturnCode_t::RETCODE_OK)
            {
                if (info.valid_data)
                {
                    long long trecv = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                    long long tsend = std::stoll(content_.message());
                    long long elapsed = trecv - tsend;
                    latency += (elapsed - latency) / ++samples;
                    // std::cerr << "Message: " << content_.message() << " with index: " << content_.index()
                    //             << " RECEIVED." << std::endl;
                }
            }
        }

        SimpleTopic content_;

        double latency;
        int samples;

    } listener_;

public:

    SimpleSubscriber()
        : participant_(nullptr)
        , subscriber_(nullptr)
        , topic_(nullptr)
        , reader_(nullptr)
        , type_(new SimpleTopicPubSubType())
    {
    }

    virtual ~SimpleSubscriber()
    {
        if (reader_ != nullptr)
        {
            subscriber_->delete_datareader(reader_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        if (subscriber_ != nullptr)
        {
            participant_->delete_subscriber(subscriber_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the subscriber
    bool init()
    {
        DomainParticipantQos participantQos;
        participantQos.name("Participant_subscriber");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the subscriptions Topic
        topic_ = participant_->create_topic("topic", "SimpleTopic", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Subscriber
        subscriber_ = participant_->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);

        if (subscriber_ == nullptr)
        {
            return false;
        }

        // Create the DataReader
        reader_ = subscriber_->create_datareader(topic_, DATAREADER_QOS_DEFAULT, &listener_);

        if (reader_ == nullptr)
        {
            return false;
        }

        return true;
    }

    //!Run the Subscriber
    void run(int duration)
    {
        std::this_thread::sleep_for(std::chrono::seconds(duration));
    }

    double getLatency()
    {
        return listener_.latency;
    }
};

double latencies[20];

void run_subscriber(int tid, int duration)
{
    // std::cerr << "Starting subscriber." << std::endl;

    SimpleSubscriber* sub = new SimpleSubscriber();
    if(sub->init())
    {
        sub->run(duration);
    }
    latencies[tid] = sub->getLatency();

    delete sub;
}

int main(
        int argc,
        char** argv)
{
    if (argc < 3)
        std::cerr << "Usage: sub <Npublishers/Nsubscribers> <duration>\n";
    int nodes = std::stoi(argv[1]);
    int seconds = std::stoi(argv[2]);

    std::thread th[10];
    for (int i = 0; i < nodes; i++)
        th[i] = std::thread(run_subscriber, i, seconds);
    for (int i = 0; i < nodes; i++)
        th[i].join();

    double result = 0;
    for (int i = 0; i < nodes; i++)
        result += latencies[i];
    result /= nodes;
    std::cout << result << "\n";

    return 0;
}
