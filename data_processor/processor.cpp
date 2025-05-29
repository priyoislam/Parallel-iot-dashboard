#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <omp.h>
#include <json/json.h> 
#include "mqtt/async_client.h"

const std::string SERVER_ADDRESS{"tcp://localhost:1883"};
const std::string CLIENT_ID{"cpp_subscriber"};
const std::string TOPIC{"iot/data"};

std::unordered_map<std::string, int> device_thread_map;
std::mutex map_mutex;

void process_data(const std::string& device_id, const Json::Value& data) {
    // Simulate parallel workload (e.g., ML prep, filtering, stats)
    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            #pragma omp task
            {
                std::cout << "[Thread " << omp_get_thread_num() << "] "
                          << "Processing for device: " << device_id
                          << " | Temp: " << data["temperature"].asFloat()
                          << std::endl;
            }
        }
    }
}

class callback : public virtual mqtt::callback {
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

        std::string device_id = root["device_id"].asString();

        {
            std::lock_guard<std::mutex> lock(map_mutex);
            if (device_thread_map.find(device_id) == device_thread_map.end()) {
                int thread_id = device_thread_map.size();
                device_thread_map[device_id] = thread_id;
                std::cout << "New device: " << device_id << " assigned to thread " << thread_id << std::endl;
            }
        }

        process_data(device_id, root);
    }
};

int main() {
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
    callback cb;
    client.set_callback(cb);

    mqtt::connect_options connOpts;
    client.connect(connOpts)->wait();
    client.subscribe(TOPIC, 1)->wait();

    std::cout << "Subscribed to " << TOPIC << ", waiting for data..." << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}