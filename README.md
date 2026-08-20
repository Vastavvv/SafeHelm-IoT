# SafeHelm-IoT

## ESP32-Based IoT Smart Helmet for Real-Time Worker Safety Monitoring

SafeHelm-IoT is an IoT-based smart helmet system designed to improve worker safety by continuously monitoring environmental and physiological conditions.

The system uses an ESP32 to collect sensor data, process safety conditions, trigger local alerts, and transmit real-time sensor readings to a web-based monitoring dashboard over Wi-Fi.

## Features

- Real-time MQ-2 gas detection
- Gas threshold-based emergency alert
- MPU6050-based tilt and fall detection
- MAX30105-based heart-rate monitoring
- Local buzzer and LED emergency alerts
- ESP32 Wi-Fi connectivity
- Real-time web dashboard
- Live sensor data visualization
- Emergency alert display on dashboard
- Sensor history and monitoring interface

## Hardware Components

- ESP32 Dev Module
- MQ-2 Gas Sensor
- MPU6050 Accelerometer and Gyroscope
- MAX30105 Heart Rate Sensor
- Buzzer
- LED
- Battery and Boost Converter

## Pin Connections

| Component | ESP32 Pin |
|---|---|
| MQ-2 Analog Output | GPIO 35 |
| Buzzer | GPIO 27 |
| LED | GPIO 26 |
| MPU6050 SDA | GPIO 21 |
| MPU6050 SCL | GPIO 22 |
| MAX30105 SDA | GPIO 21 |
| MAX30105 SCL | GPIO 22 |

## System Workflow

```text
Sensors
   ↓
ESP32
   ↓
Data Processing
   ↓
Safety Evaluation
   ↓
┌───────────────┬────────────────┐
│ Local Alerts  │ Wi-Fi Dashboard│
│ Buzzer + LED  │ Live Monitoring│
└───────────────┴────────────────┘
