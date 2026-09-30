# SmartSense

## IoT-Based Smart Home Environmental Monitoring & Automation System Using ESP32

SmartSense is an IoT-based smart home environmental monitoring and automation system developed using the ESP32 microcontroller. The system collects environmental data from sensors and provides real-time monitoring through a web-based interface.

## Features

* 🌡️ Real-time temperature monitoring
* 💧 Humidity monitoring
* 💡 Light-level monitoring
* 🚶 Motion detection
* 🌫️ Air-quality monitoring
* 📡 ESP32 Wi-Fi connectivity
* 🌐 Web-based monitoring dashboard
* ☁️ Firebase Realtime Database integration *(planned/upcoming)*
* ⚙️ Smart home automation *(planned/upcoming)*

## Hardware

* ESP32 DevKit V1
* DHT11 Temperature & Humidity Sensor
* Light Sensor
* Motion Sensor
* Air Quality Sensor
* Jumper Wires
* Breadboard
* USB Cable

## Software & Technologies

* ESP32
* Arduino IDE
* C/C++
* Wi-Fi
* HTML
* CSS
* JavaScript
* Firebase Realtime Database
* Web Dashboard

## System Architecture

```text
Sensors
   │
   ▼
ESP32
   │
   ├── Temperature
   ├── Humidity
   ├── Light Level
   ├── Motion
   └── Air Quality
   │
   ▼
Wi-Fi
   │
   ▼
Web Dashboard
   │
   ▼
Firebase Realtime Database
```

## Current Implementation

The current version successfully demonstrates:

* ESP32 initialization
* Wi-Fi connection
* DHT11 temperature and humidity monitoring
* Sensor data processing
* ESP32-hosted web server
* Real-time display of sensor readings

Example output:

```text
Temperature : 30.30 °C
Humidity    : 69.00 %
Light Level : 50
Motion      : DETECTED
Air Quality : 70
```

## Project Structure

```text
SmartSense/
│
├── SmartSense.ino
├── README.md
├── LICENSE
└── images/
    └── system-architecture.png
```

## Future Improvements

* Firebase Realtime Database integration
* Live sensor graphs
* Mobile-friendly dashboard
* Automated device control
* Sensor threshold alerts
* Remote monitoring
* Historical sensor data
* Smart home automation
* Improved security and authentication

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/YOUR_USERNAME/SmartSense-ESP32.git
```

### 2. Open the project

Open the project in Arduino IDE or VS Code with the ESP32 development environment.

### 3. Configure Wi-Fi

Update the Wi-Fi credentials in the source code according to your local network.

### 4. Connect the ESP32

Connect the ESP32 DevKit V1 to your computer using a USB cable.

### 5. Upload the code

Select the appropriate ESP32 board and COM port, then upload the program.

### 6. Monitor the system

Open the Serial Monitor and observe the sensor readings and ESP32 IP address.

Open the displayed IP address in a web browser connected to the same Wi-Fi network.

## Project Status

🚧 **Under Development**

SmartSense is being developed as an IoT-based smart home environmental monitoring and automation project.

## Author

**Daniel Sherin**

B.Tech Computer Science & Engineering

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

