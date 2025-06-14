
#include <iostream>
#include <string>
#include <sstream>
#include <json/json.h>
#include "mqtt/async_client.h"
#include "thread_pool.hpp"
#include "concurrent_queue.hpp"
#include "connectdb.hpp"
#include <curl/curl.h>  
const std::string SERVER_ADDRESS{"tcp://localhost:1883"};
const std::string CLIENT_ID{"cpp_subscriber"};
const std::string TOPIC{"iot/data"};
const int NUM_WORKERS = 4;

struct Message {
    std::string device_id;
    Json::Value payload;
};

ConcurrentQueue<Message> message_queue;

// Converts a JSON payload (received from MQTT) into InfluxDB Line Protocol format
std::string json_to_influx_line_protocol(const Json::Value& payload) {
    std::ostringstream oss;

    // Measurement: "sensor_data"
    oss << "sensor_data";

    // Tags: device_id and region (from the outer JSON object)
    if (payload.isMember("device_id") && payload["device_id"].isString()) {
        oss << ",device_id=" << payload["device_id"].asString();
    }
    if (payload.isMember("region") && payload["region"].isString()) {
        oss << ",region=" << payload["region"].asString();
    }

    oss << " "; // Space separator between tags and fields

    // Fields: Values from the "Sensor Data" nested JSON object
    Json::Value sensor_data = payload["Sensor Data"];
    bool first_field = true; // Helper to manage comma separation for fields

    if (sensor_data.isMember("air_quality_index") && sensor_data["air_quality_index"].isNumeric()) {
        oss << "air_quality_index=" << sensor_data["air_quality_index"].asInt();
        first_field = false;
    }
    if (sensor_data.isMember("co2_level") && sensor_data["co2_level"].isNumeric()) {
        if (!first_field) oss << ",";
        oss << "co2_level=" << sensor_data["co2_level"].asDouble();
        first_field = false;
    }
    if (sensor_data.isMember("humidity") && sensor_data["humidity"].isNumeric()) {
        if (!first_field) oss << ",";
        oss << "humidity=" << sensor_data["humidity"].asDouble();
        first_field = false;
    }
    if (sensor_data.isMember("noise_level") && sensor_data["noise_level"].isNumeric()) {
        if (!first_field) oss << ",";
        oss << "noise_level=" << sensor_data["noise_level"].asDouble();
        first_field = false;
    }
    if (sensor_data.isMember("temperature") && sensor_data["temperature"].isNumeric()) {
        if (!first_field) oss << ",";
        oss << "temperature=" << sensor_data["temperature"].asDouble();
        first_field = false;
    }

    // Timestamp: Convert Unix seconds timestamp to nanoseconds (InfluxDB default precision)
    if (payload.isMember("Timestamp") && payload["Timestamp"].isNumeric()) {
        long long timestamp_seconds = payload["Timestamp"].asInt64(); // Get timestamp as 64-bit integer
        long long timestamp_nanoseconds = timestamp_seconds * 1000000000LL; // Convert to nanoseconds
        oss << " " << timestamp_nanoseconds;
    } else {
        // If no timestamp is provided in the JSON, InfluxDB will use the server's timestamp.
        // For current time in nanoseconds:
        // oss << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(
        //     std::chrono::system_clock::now().time_since_epoch()
        // ).count();
    }

    return oss.str();
}

// Function executed by each worker thread in the thread pool
void process_data(const Message& msg) {
    std::cout << "[Thread " << std::this_thread::get_id() << "] Processing data for device: "
              << msg.device_id << std::endl;

    // Convert the received JSON payload to InfluxDB Line Protocol
    std::string line_protocol = json_to_influx_line_protocol(msg.payload);

    // Attempt to write the data to InfluxDB
    if (!write_influx_data(line_protocol)) {
        std::cerr << "[Thread " << std::this_thread::get_id() << "] Failed to write data for device: "
                  << msg.device_id << " to InfluxDB.\n";
    }
}

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
    curl_global_init(CURL_GLOBAL_ALL);
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
    curl_global_cleanup();

    return 0;
}