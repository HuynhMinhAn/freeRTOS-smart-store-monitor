# FreeRTOS Smart Store Monitor

This project implements a real-time smart store monitoring system using ESP32 and FreeRTOS. It tracks customer entry/exit, measures temperature and humidity, and provides fire alerts with OLED display and buzzer support.

## Author
**Huynh Minh An**

## Features
- Dual ultrasonic sensors to count people entering and leaving
- DHT22 sensor to monitor temperature and humidity
- OLED screen displays:
  - Current number of people
  - Total entries
  - Temperature and humidity
  - Fire alert if temperature exceeds threshold
- Fire alert with buzzer and LED
- Uses FreeRTOS: 5 tasks + 1 software timer + semaphore for alarm handling

## Hardware
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

## Wiring Diagram

View the complete wiring diagram on Wokwi:  
🔗 [https://wokwi.com/projects/417785142236832769](https://wokwi.com/projects/417785142236832769)

> You can interact with the circuit and simulate it directly in your browser.


## Demo

Watch the full project demonstration on YouTube:  
🔗 [https://www.youtube.com/watch?v=L_Eo5bQP7rU&t=3s](https://www.youtube.com/watch?v=L_Eo5bQP7rU&t=3s)



## RTOS Task Overview
| Task                  | Priority | Description                          |
|-----------------------|----------|--------------------------------------|
| EntryTask             | 2        | Detects people entering              |
| ExitTask              | 2        | Detects people leaving               |
| SendSemaphoreTask     | 1        | Reads DHT, sends alarm if needed     |
| ReceiveSemaphoreTask  | 3        | Triggers buzzer alert                |
| LightControlTask      | 3        | Controls LED based on light/button   |
| LCD TimerCallback     | -        | Updates OLED display every 2s        |

## 📚 Required Libraries

To run this project successfully on ESP32 using Arduino IDE, please install the following libraries:

| Library Name                | Install From                    | Notes                                              |
|----------------------------|----------------------------------|----------------------------------------------------|
| **Adafruit SSD1306**       | Library Manager                  | OLED 128x64 display driver                         |
| **Adafruit GFX Library**   | Library Manager                  | Core graphics library for Adafruit displays        |
| **DHT sensor library**     | Library Manager (by Adafruit)    | Reads temperature & humidity from DHT22            |
| **Adafruit Unified Sensor**| Library Manager                  | Required by DHT sensor library                     |
| **Wire**                   | Built-in with Arduino IDE        | I2C communication                                  |
| **FreeRTOS (ESP32 Core)**  | Built-in with ESP32 board core   | For multitasking (FreeRTOS is built-in)            |
| **Buzzer** (custom)        | *Custom or implement tone()*     | You can use tone() instead if Buzzer.h is missing  |

> Make sure these libraries are available in your Arduino environment before compiling.


## Getting Started
1. Open `code/main.ino` in Arduino IDE
2. Select ESP32 board and install required libraries:
   - Adafruit_SSD1306
   - DHT
   - Buzzer
3. Flash and run

## 🪪 License
MIT License – by Huynh Minh An
