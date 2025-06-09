cmake_minimum_required(VERSION 3.10)
project(parallel_iot_dashboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Paho MQTT C++ library
find_package(PahoMqttCpp REQUIRED)
# Find JsonCpp library
#find_package(JsonCpp REQUIRED)



# Source files
set(SOURCES
    main.cpp
    thread_pool.hpp
    concurrent_queue.hpp
)

add_executable(mqtt_subscriber ${SOURCES})

# Link libraries
target_link_libraries(mqtt_subscriber
    PahoMqttCpp::paho-mqttpp3
    ${JSONCPP_LIBRARIES}
    pthread
)