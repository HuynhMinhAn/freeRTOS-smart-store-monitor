# 🏪 FreeRTOS Smart Store Monitor

This project implements a real-time smart store monitoring system using ESP32 and FreeRTOS. It tracks customer entry/exit, measures temperature and humidity, and provides fire alerts with OLED display and buzzer support.

## 👤 Author
**Huynh Minh An**

## 🚀 Features
- Dual ultrasonic sensors to count people entering and leaving
- DHT22 sensor to monitor temperature and humidity
- OLED screen displays:
  - Current number of people
  - Total entries
  - Temperature and humidity
  - Fire alert if temperature exceeds threshold
- Fire alert with buzzer and LED
- Uses FreeRTOS: 5 tasks + 1 software timer + semaphore for alarm handling

## 🧰 Hardware
| Device       | Quantity |
|--------------|----------|
| ESP32 DevKit | 1        |
| DHT22        | 1        |
| HC-SR04      | 2        |
| OLED 128x64  | 1        |
| Buzzer       | 1        |
| LED          | 2        |
| Button/Sensor| Optional |
| Jumper wires | Breadboard, etc. |

## 📷 Wiring Diagram
![Wiring](docs/wiring_diagram.png)

## 📽️ Demo
![Demo](docs/demo.gif)

## 🧵 RTOS Task Overview
| Task                  | Priority | Description                          |
|-----------------------|----------|--------------------------------------|
| EntryTask             | 2        | Detects people entering              |
| ExitTask              | 2        | Detects people leaving               |
| SendSemaphoreTask     | 1        | Reads DHT, sends alarm if needed     |
| ReceiveSemaphoreTask  | 3        | Triggers buzzer alert                |
| LightControlTask      | 3        | Controls LED based on light/button   |
| LCD TimerCallback     | -        | Updates OLED display every 2s        |

## 🛠️ Getting Started
1. Open `code/main.ino` in Arduino IDE
2. Select ESP32 board and install required libraries:
   - Adafruit_SSD1306
   - DHT
   - Buzzer
3. Flash and run

## 🪪 License
MIT License – by Huynh Minh An
