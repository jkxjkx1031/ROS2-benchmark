#include "SimpleTopicPubSubTypes.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/DataWriterListener.hpp>

using namespace eprosima::fastdds::dds;
using namespace std::chrono;

class SimplePublisher
{
private:

    SimpleTopic content_;

    DomainParticipant* participant_;

    Publisher* publisher_;

    Topic* topic_;

    DataWriter* writer_;

    TypeSupport type_;

    class PubListener : public DataWriterListener
    {
    public:

        PubListener()
            : matched_(0)
        {
        }

        ~PubListener() override
        {
        }

        void on_publication_matched(
                DataWriter*,
                const PublicationMatchedStatus& info) override
        {
            if (info.current_count_change == 1)
            {
                matched_ = info.total_count;
                std::cerr << "Publisher matched." << std::endl;
            }
            else if (info.current_count_change == -1)
            {
                matched_ = info.total_count;
                std::cerr << "Publisher unmatched." << std::endl;
            }
            else
            {
                std::cerr << info.current_count_change
                        << " is not a valid value for PublicationMatchedStatus current count change." << std::endl;
            }
        }

        std::atomic_int matched_;

    } listener_;

public:

    SimplePublisher()
        : participant_(nullptr)
        , publisher_(nullptr)
        , topic_(nullptr)
        , writer_(nullptr)
        , type_(new SimpleTopicPubSubType())
    {
    }

    virtual ~SimplePublisher()
    {
        if (writer_ != nullptr)
        {
            publisher_->delete_datawriter(writer_);
        }
        if (publisher_ != nullptr)
        {
            participant_->delete_publisher(publisher_);
        }
        if (topic_ != nullptr)
        {
            participant_->delete_topic(topic_);
        }
        DomainParticipantFactory::get_instance()->delete_participant(participant_);
    }

    //!Initialize the publisher
    bool init()
    {
        content_.index(0);
        content_.message("_hereis10B");

        DomainParticipantQos participantQos;
        participantQos.name("Participant_publisher");
        participant_ = DomainParticipantFactory::get_instance()->create_participant(0, participantQos);

        if (participant_ == nullptr)
        {
            return false;
        }

        // Register the Type
        type_.register_type(participant_);

        // Create the publications Topic
        topic_ = participant_->create_topic("topic", "SimpleTopic", TOPIC_QOS_DEFAULT);

        if (topic_ == nullptr)
        {
            return false;
        }

        // Create the Publisher
        publisher_ = participant_->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);

        if (publisher_ == nullptr)
        {
            return false;
        }

        // Create the DataWriter
        writer_ = publisher_->create_datawriter(topic_, DATAWRITER_QOS_DEFAULT, &listener_);

        if (writer_ == nullptr)
        {
            return false;
        }
        return true;
    }

    //!Send a publication
    bool publish()
    {
        if (listener_.matched_ > 0)
        {
            content_.index(content_.index() + 1);
            content_.message(std::to_string(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count()));
            writer_->write(&content_);
            return true;
        }
        return false;
    }

    //!Run the Publisher
    void run(int nodes, int samples, int interval)
    {
        while (listener_.matched_ < nodes) /* wait */;
        int samples_sent = 0;
        while (samples_sent < samples)
        {
            if (publish())
            {
                samples_sent++;
                // std::cout << "Message: " << content_.message() << " with index: " << content_.index()
                //             << " SENT" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    }
};

int main(
        int argc,
        char** argv)
{
    // std::cerr << "Starting publisher." << std::endl;
    
    if (argc < 4)
        std::cerr << "Usage: pub <Npublishers/Nsubscribers> <Nsamples> <sendInterval>\n";
    int nodes = std::stoi(argv[1]);
    int samples = std::stoi(argv[2]);
    int interval = std::stoi(argv[3]);

    SimplePublisher* pub = new SimplePublisher();
    if(pub->init())
    {
        pub->run(nodes, samples, interval);
    }

    delete pub;
    return 0;
}