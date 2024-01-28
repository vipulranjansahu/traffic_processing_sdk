#include <iostream>
#include <string>
#include <csignal>
#include <cpr/cpr.h>
#include <librdkafka/rdkafkacpp.h>

static volatile sig_atomic_t run = 1;

static void sigterm(int sig) {
    run = 0;
}

class TrafficDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
    void dr_cb(RdKafka::Message& message) {
        if (message.err())
            std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
        else
            std::cerr << "% Message delivered to topic " << message.topic_name()
            << " [" << message.partition() << "] at offset "
            << message.offset() << std::endl;
    }
};

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <brokers> <topic>\n";
        exit(1);
    }

    std::string brokers = argv[1];
    std::string topic = argv[2];

    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    std::string errstr;

    if (conf->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    TrafficDeliveryReportCb traffic_dr_cb;

    if (conf->set("dr_cb", &traffic_dr_cb, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    RdKafka::Producer* producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    delete conf;

    std::cout << "% Type URL and hit enter to make an HTTP request and produce message to Kafka.\n";

    for (std::string url; run && std::getline(std::cin, url);) {
        if (url.empty()) {
            producer->poll(0);
            continue;
        }

        // Make an HTTP request
        cpr::Response r = cpr::Get(cpr::Url{ url });

        // Create a string to hold both the URL and response data
        std::string messageData = "URL: " + url + "\nResponse: " + r.text;

        // Send/Produce message
    retry:
        RdKafka::ErrorCode err = producer->produce(
            topic,
            RdKafka::Topic::PARTITION_UA,
            RdKafka::Producer::RK_MSG_COPY,
            const_cast<char*>(messageData.c_str()), messageData.size(),
            NULL, 0,
            0,
            NULL,
            NULL);

        if (err != RdKafka::ERR_NO_ERROR) {
            std::cerr << "% Failed to produce to topic " << topic << ": "
                << RdKafka::err2str(err) << std::endl;

            if (err == RdKafka::ERR__QUEUE_FULL) {
                producer->poll(1000);
                goto retry;
            }
        }
        else {
            std::cerr << "% Enqueued message (" << messageData.size() << " bytes) "
                << "for topic " << topic << std::endl;
        }

        producer->poll(0);
    }

    std::cerr << "% Flushing final messages..." << std::endl;
    producer->flush(10 * 1000);

    if (producer->outq_len() > 0)
        std::cerr << "% " << producer->outq_len() << " message(s) were not delivered" << std::endl;

    delete producer;

    return 0;
}
