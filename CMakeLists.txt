cmake_minimum_required(VERSION 3.10)
project(parallel_iot_dashboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Paho MQTT C++ library
find_package(PahoMqttCpp REQUIRED)
# Find JsonCpp library

find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONCPP REQUIRED jsoncpp)
find_package(CURL REQUIRED)
# Source files
set(SOURCES
    main.cpp
    thread_pool.hpp
    concurrent_queue.hpp
    connectdb.cpp
)

add_executable(prll_app ${SOURCES})

include_directories(${JSONCPP_INCLUDE_DIRS})
# Link libraries
target_link_libraries(prll_app
    PahoMqttCpp::paho-mqttpp3
    ${JSONCPP_LIBRARIES}
    pthread
    CURL::libcurl
)
