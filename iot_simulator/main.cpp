#include <iostream>
#include <string>
#include <sstream>
#include <json/json.h>
#include "mqtt/async_client.h"
#include "thread_pool.hpp"
#include "concurrent_queue.hpp"
#include "connectdb.hpp"

const std::string SERVER_ADDRESS{"tcp://localhost:1883"};
const std::string CLIENT_ID{"cpp_subscriber"};
const std::string TOPIC{"iot/data"};
const int NUM_WORKERS = 4;

struct Message {
    std::string device_id;
    Json::Value payload;
};

ConcurrentQueue<Message> message_queue;

// void process_data(const Message& msg) {
//     std::cout << "[Thread " << std::this_thread::get_id() << "] "
//               <<  msg.payload
//               << std::endl;
// }

class Callback : public virtual mqtt::callback {
public:
    void message_arrived(mqtt::const_message_ptr msg) override {
        Json::CharReaderBuilder builder;
        Json::Value root;
        std::string errs;
        std::istringstream s(msg->to_string());
        if (!parseFromStream(builder, s, &root, &errs)) {
            std::cerr << "Failed to parse JSON: " << errs << std::endl;
            return;
        }

        Message m;
        m.device_id = root["device_id"].asString();
        m.payload = root;
        message_queue.push(m);
    }
};

int main() {

      if (!test_influxdb_connection()) {
        std::cerr << "Exiting: could not connect to InfluxDB.\n";
        return 1;
    }

    // Start thread pool
    ThreadPool pool;
    pool.start(NUM_WORKERS, [] {
        while (true) {
            Message msg = message_queue.pop();
            process_data(msg);
        }
    });

    // Start MQTT client
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
    Callback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    client.connect(connOpts)->wait();
    client.subscribe(TOPIC, 1)->wait();

    std::cout << "Subscribed to " << TOPIC << " | Waiting for messages...\n";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    pool.join(); // never reached in this example

    return 0;
}