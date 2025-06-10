
// connectdb.cpp
#include "connectdb.hpp"
#include <iostream>
#include <curl/curl.h> // Required for libcurl operations
#include <sstream>     // For string stream manipulation

// --- INFLUXDB CONFIGURATION ---
// IMPORTANT: Replace these with your actual InfluxDB 2.x details
const std::string INFLUXDB_URL = "http://localhost:8086/api/v2/write"; // InfluxDB 2.x write endpoint
const std::string INFLUXDB_ORG = "myorg"; // Your InfluxDB organization name
const std::string INFLUXDB_BUCKET = "sensor_data"; // Your InfluxDB bucket name
const std::string INFLUXDB_TOKEN = "my-super-secret-auth-token"; // Your InfluxDB API token
// ----------------------------

static bool INFLUXDB_DEBUG_VERBOSE = true;

// Callback function for curl to write response data (optional for writes, but good for error logging)
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Tests the connection to the InfluxDB /health endpoint
bool test_influxdb_connection() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl for health check\n";
        return false;
    }

    const std::string health_url = "http://localhost:8086/health"; // InfluxDB health endpoint

    curl_easy_setopt(curl, CURLOPT_URL, health_url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); // Important for multi-threaded applications to avoid signal issues

    // Set Authorization header (often required even for /health in InfluxDB 2.x)
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Token " + INFLUXDB_TOKEN).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    long http_code = 0;
    std::string readBuffer; // Buffer to store HTTP response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code >= 200 && http_code < 300) { // HTTP 2xx indicates success
            success = true;
            std::cout << "Successfully connected to InfluxDB /health endpoint (HTTP " << http_code << ").\n";
        } else {
            std::cerr << "InfluxDB /health check failed with HTTP status: " << http_code << ". Response: " << readBuffer << "\n";
        }
    } else {
        std::cerr << "Connection to InfluxDB failed during health check: " << curl_easy_strerror(res) << "\n";
    }

    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

// Writes data to InfluxDB using the Line Protocol format
bool write_influx_data(const std::string& line_protocol_data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl for data write\n";
        return false;
    }

    // Construct the full InfluxDB write URL with organization, bucket, and precision (nanoseconds)
    std::string full_url = INFLUXDB_URL + "?org=" + INFLUXDB_ORG + "&bucket=" + INFLUXDB_BUCKET + "&precision=ns";

    curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L); // Set as POST request
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, line_protocol_data.c_str()); // Set the data payload
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, line_protocol_data.length()); // Set the size of the payload
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L); // Important for multi-threaded applications

     if (INFLUXDB_DEBUG_VERBOSE) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: text/plain; charset=utf-8"); // Specify content type for Line Protocol
    headers = curl_slist_append(headers, ("Authorization: Token " + INFLUXDB_TOKEN).c_str()); // Set authorization token
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    long http_code = 0;
    std::string readBuffer; // Buffer to store HTTP response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code >= 200 && http_code < 300) { // HTTP 2xx indicates success
            success = true;
            // Uncomment the line below for very verbose success logging
            // std::cout << "[InfluxDB Write] Success (HTTP " << http_code << ")\n";
        } else {
            std::cerr << "[InfluxDB Write] Failed with HTTP status: " << http_code << ". Response: " << readBuffer << ". Data: '" << line_protocol_data << "'\n";
        }
    } else {
        std::cerr << "[InfluxDB Write] Failed: " << curl_easy_strerror(res) << ". Data: '" << line_protocol_data << "'\n";
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}

