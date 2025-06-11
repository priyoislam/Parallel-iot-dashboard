// connectdb.hpp
// connectdb.hpp
#ifndef CONNECTDB_HPP
#define CONNECTDB_HPP

#include <string>
#include <json/json.h> // Assuming jsoncpp library

// Function to test connection to InfluxDB
bool test_influxdb_connection();

// Function to write data to InfluxDB
// Takes data in InfluxDB Line Protocol format
bool write_influx_data(const std::string& line_protocol_data);

#endif // CONNECTDB_HPP