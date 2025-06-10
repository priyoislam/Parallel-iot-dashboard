#include <iostream>
#include <curl/curl.h>

bool test_influxdb_connection() {   


    

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl\n";
        return false;
    }

    const std::string influx_url = "http://localhost:8086/health";
    const std::string token = "my-super-secret-auth-token";  // optional, can omit for health

    curl_easy_setopt(curl, CURLOPT_URL, influx_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

    // Set Authorization header (optional for /health)
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Token " + token).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    bool success = (res == CURLE_OK);

    if (!success) {
        std::cerr << "Connection to InfluxDB failed: " << curl_easy_strerror(res) << "\n";
    } else {
        std::cout << "Successfully connected to InfluxDB /health endpoint.\n";
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}



void process_data(const Message& msg) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL\n";
        return;
    }

    std::ostringstream data;
    Json::Value sensorData = msg.payload["Sensor Data"];
    std::string region = msg.payload["region"].asString();
    std::string device_id = msg.payload["device_id"].asString();
    long timestamp = msg.payload["Timestamp"].asInt64();

    // Format line protocol
    data << "sensor_data"
         << ",device_id=" << device_id
         << ",region=" << region
         << " "
         << "air_quality_index=" << sensorData["air_quality_index"].asInt() << ","
         << "co2_level=" << sensorData["co2_level"].asDouble() << ","
         << "humidity=" << sensorData["humidity"].asDouble() << ","
         << "noise_level=" << sensorData["noise_level"].asDouble() << ","
         << "temperature=" << sensorData["temperature"].asDouble()
         << " " << timestamp << "000000000"; // nanoseconds

    std::string postData = data.str();

    std::string url = "http://localhost:8086/api/v2/write?org=myorg&bucket=sensor_data&precision=ns";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Authorization: Token my-super-secret-auth-token");
    headers = curl_slist_append(headers, "Content-Type: text/plain");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "InfluxDB write failed: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "[Thread " << std::this_thread::get_id() << "] Wrote data for device: " << device_id << std::endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}