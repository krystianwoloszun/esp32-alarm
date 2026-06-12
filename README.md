# ESP32 Movement Detection System

An ESP32-based movement detection system using an ultrasonic sensor, TFT display, potentiometer, buzzer, and LED indicator. The system measures distance, detects motion based on a configurable threshold, counts detections, and sends notifications via a Telegram bot.

---
<img width="2048" height="1536" alt="-PlJTbow" src="https://github.com/user-attachments/assets/cc13a936-ef42-40b4-953a-b4454660d6bb" />


## Features

* Real-time distance measurement using HC-SR04 ultrasonic sensor
* Adjustable detection threshold (20–200 cm) using a potentiometer
* Movement detection system with state tracking
* Audible alert using a buzzer
* LED indicator for detection status
* TFT display visualization (distance, counter, threshold)
* Telegram notifications on detection (rate-limited to 1 message per minute)

---

## Hardware Components

* ESP32 development board
* HC-SR04 ultrasonic sensor
* TFT SPI display
* Potentiometer (analog input)
* Buzzer
* LED (GPIO 2)

---

## Wiring Diagram

### HC-SR04

| HC-SR04 | ESP32   |
| ------- | ------- |
| VCC     | 5V      |
| GND     | GND     |
| TRIG    | GPIO 26 |
| ECHO    | GPIO 14 |

---

### Potentiometer

| Potentiometer | ESP32   |
| ------------- | ------- |
| VCC           | 3.3V    |
| GND           | GND     |
| OUT           | GPIO 27 |

---

### Buzzer

| Buzzer | ESP32   |
| ------ | ------- |
| +      | GPIO 32 |
| -      | GND     |

---

### LED

| LED     | ESP32  |
| ------- | ------ |
| Anode   | GPIO 2 |
| Cathode | GND    |

---

### TFT Display (SPI)

| TFT Pin    | ESP32         |
| ---------- | ------------- |
| SDO / MISO | Not connected |
| SCK        | GPIO 18       |
| SDI / MOSI | GPIO 23       |
| DC         | GPIO 16       |
| RESET      | GPIO 17       |
| CS         | GPIO 5        |
| GND        | GND           |
| VCC        | 3.3V          |
| LED        | 3.3V          |

> Uses SPI interface and requires correct configuration of **TFT_eSPI** library (`User_Setup.h`).

---

## Required Libraries

Install the following libraries in Arduino IDE:

* TFT_eSPI
* WiFi (ESP32 core)
* WiFiClientSecure
* UniversalTelegramBot
* SPI

---

## Configuration

Before uploading, configure your credentials:

```cpp
const char* ssid = "your_ssid";
const char* password = "your_password";

#define BOT_TOKEN "your_bot_token"
#define CHAT_ID "your_chat_id"
```

---

## Telegram Bot Setup

Telegram integration is handled externally.

Setup guide:

https://medium.com/@Elhazin/creating-a-telegram-bot-55e6ca4e337d

---

## How It Works

1. The potentiometer sets a detection threshold (20–200 cm).
2. The HC-SR04 sensor continuously measures distance.
3. If the measured distance is below the threshold:

   * LED turns ON
   * Buzzer emits a short beep
   * Movement counter increases
   * System enters "detected" state to avoid duplicate triggers
4. When object leaves the range, system resets detection state.
5. Telegram notification is sent at most once per minute.
6. TFT display shows:

   * Current distance
   * Detection counter
   * Threshold value

---

## System Startup

On startup the system:

* Connects to WiFi
* Sends a startup message to Telegram
* Initializes TFT display
* Starts monitoring distance

---

## TFT Display Output

The display shows:

* DISTANCE (cm)
* COUNTER (number of detections)
* THRESHOLD (adjustable via potentiometer)

---


## Authors

* Krystian Wołoszun
* Karolina Żurek
