# Parallel IoT Data Simulation and Real-Time Dashboard Visualization

**Group Members:**  
Moinul Laskar (2404679) & Nicolas Ragnell (39168)

---

## Project Overview

As smart cities evolve, efficient and scalable monitoring of environmental parameters such as temperature, humidity, and noise is essential. This project aims to build a system that simulates multiple IoT devices generating sensor data in parallel, processes the data concurrently, and visualizes it in real-time through an interactive dashboard.

Our system mimics a real-world smart city by simulating thousands of IoT sensors distributed across different city regions. The project emphasizes **parallel computing** to handle simultaneous data acquisition and processing, enabling scalability and timely insights.

---

## Features

- **IoT Device Simulation:** Simulates numerous IoT sensors generating data streams in parallel.
- **Parallel Data Processing:** Utilizes multi-threading (POSIX Threads or OpenMP) to process incoming sensor data efficiently.
- **MQTT Messaging:** Communicates sensor data using an MQTT broker to simulate real-world IoT communication.
- **Real-Time Dashboard:** Visualizes processed data interactively using Plotly Dash, providing insights and analytics for smart city management.

---

## Tools and Technologies

- **Programming Language:** C++
- **Parallelism:** POSIX Threads or OpenMP
- **IoT Simulation:** (To be decided)
- **Message Broker:** MQTT (Mosquitto recommended)
- **Dashboard Platform:** Plotly Dash (Python)

---

## Getting Started

### Prerequisites

- C++17 compatible compiler
- [Paho MQTT C++ client library](https://github.com/eclipse/paho.mqtt.cpp)
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp)
- Mosquitto MQTT broker installed and running locally
- Python 3 with Plotly Dash for the dashboard component

# Mosquitto MQTT Docker Setup
### Pull the Official Image
```bash
docker pull eclipse-mosquitto

```
### Start Mosquitto with Anonymous Access
```bash
# Create config directory
mkdir -p ./mosquitto/config

# Generate minimal config (open access)
echo -e 'listener 1883 0.0.0.0\nallow_anonymous true' > ./mosquitto/config/mosquitto.conf
```
### Run container with config volume
```bash
docker run -d \
  -p 1883:1883 \
  -v $(pwd)/mosquitto/config:/mosquitto/config \
  --name mosquitto \
  eclipse-mosquitto
```


# Iot_SData_imulator

Instruction inside the folder


