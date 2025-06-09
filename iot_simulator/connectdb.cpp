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