
// connectdb.cpp
#include "connectdb.hpp"
#include <iostream>
#include <curl/curl.h> // Required for libcurl operations
#include <sstream>     // For string stream manipulation

// --- INFLUXDB CONFIGURATION ---
// IMPORTANT: Replace these with your actual InfluxDB 2.x details
const std::string INFLUXDB_URL = "http://localhost:8086/write"; // InfluxDB 1.x write endpoint
const std::string INFLUXDB_DB = "sensor_data";                  // Your InfluxDB 1.x database name
const std::string INFLUXDB_USER = "admin";                      // Username
const std::string INFLUXDB_PASS = "password123";  
// ----------------------------

static bool INFLUXDB_DEBUG_VERBOSE = true;

// Callback function for curl to write response data (optional for writes, but good for error logging)
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Test /ping (health) for InfluxDB 1.x
bool test_influxdb_connection() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl\n";
        return false;
    }

    const std::string ping_url = "http://localhost:8086/ping";

    curl_easy_setopt(curl, CURLOPT_URL, ping_url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // HEAD request
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    long http_code = 0;
    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 204) {
            success = true;
            std::cout << "InfluxDB is reachable at /ping (HTTP 204).\n";
        } else {
            std::cerr << "Ping failed with HTTP code " << http_code << "\n";
        }
    } else {
        std::cerr << "Ping request failed: " << curl_easy_strerror(res) << "\n";
    }

    curl_easy_cleanup(curl);
    return success;
}

// Write Line Protocol data to InfluxDB 1.x
bool write_influx_data(const std::string& line_protocol_data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl for data write\n";
        return false;
    }

    // InfluxDB 1.x write URL with DB name, username, password
    std::ostringstream url_stream;
    url_stream << INFLUXDB_URL << "?db=" << INFLUXDB_DB
               << "&u=" << INFLUXDB_USER << "&p=" << INFLUXDB_PASS;

    std::string full_url = url_stream.str();

    curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, line_protocol_data.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, line_protocol_data.length());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    if (INFLUXDB_DEBUG_VERBOSE) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: text/plain; charset=utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    long http_code = 0;
    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    bool success = false;

    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 204) {
            success = true;
            std::cout << "[InfluxDB Write] Success (HTTP 204)\n";
        } else {
            std::cerr << "[InfluxDB Write] Failed with HTTP code " << http_code << ". Response: " << readBuffer << "\n";
        }
    } else {
        std::cerr << "[InfluxDB Write] Error: " << curl_easy_strerror(res) << "\n";
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return success;
}