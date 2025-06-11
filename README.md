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

# Mosquitto MQTT(with Anonymous Access ) and Inffluxdv Setup

```bash
# Create config directory
cd broker&db

mkdir -p ./mosquitto/config   #Already done for this repo 

echo -e 'listener 1883 0.0.0.0\nallow_anonymous true' > ./mosquitto/config/mosquitto.conf #done
```
### Run container 
```bash
docker-compose pull
docker-compose up -d
```


# Run Parellel parser app

```bash
cd parellel_parser
cmake . .
make
./prll_app

```

