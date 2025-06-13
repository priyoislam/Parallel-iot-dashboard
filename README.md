# Parallel IoT Data Simulation and Real-Time Dashboard Visualization

**Group Members:**  
Moinul Laskar (2404679) & Nicolas Ragnell (39168)

---

## Project Overview

As smart cities evolve, efficient and scalable monitoring of environmental parameters such as temperature, humidity, and noise is essential. This project aims to build a system that simulates multiple IoT devices generating sensor data in parallel, processes the data concurrently, and visualizes it in real-time through an interactive dashboard.

Our system mimics a real-time smart city by simulating thousands of IoT sensors distributed across different city regions. The project emphasizes **parallel computing** to handle simultaneous data acquisition and processing, enabling scalability and timely insights.

---

## Features

- **IoT Device Simulation:** Simulates numerous IoT sensors generating data streams in parallel.
- **Parallel Data Processing:** Utilizes a custom C++ thread pool (based on std::thread) to process incoming sensor data concurrently, enabling efficient parsing, transformation, and storage in InfluxDB."
- **MQTT Messaging:** Communicates sensor data using an MQTT broker to simulate real-world IoT communication.
- **Real-Time Dashboard:** Visualizes processed data interactively using Grafana, providing insights and analytics for smart city management.

---

## Tools and Technologies

- **Programming Language:** C++
- **Parallelism:** Threads 
- **IoT Simulation:** IBA-iot-data-simulator
- **Message Broker:** MQTT (Mosquitto)
- **Database:** InfluxDB
- **Dashboard Platform:** Grafana

---

## Getting Started


# Mosquitto MQTT(with Anonymous Access ), InfluxDB and Grafana Setup

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
cmake ..    #as Cmakelist in root folder
make
./prll_app

```

IoT Data Pipeline:
───────────────────────────────────────────────────────
[IoT Sensors] ×1000s
       │
       ↓ (publish via MQTT topics)
[MQTT Broker] (e.g., Mosquitto)
       │
       ↓ (subscribe with C++ app)
[C++ Application]
   ├── Thread 1: Process Topic A → InfluxDB
   ├── Thread 2: Process Topic B → InfluxDB
   └── Thread N: ... (parallel processing)
       │
       ↓ (batch writes)
[InfluxDB Time Series Database]
       │
       ↓ (query)
[Grafana Dashboard] ← Real-time Visualization
