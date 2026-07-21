# Automatic Indicator System Using ESP32 and GPS

A low-cost, automated turn signal system designed to detect vehicle turning direction using GPS heading data and trigger appropriate LED indicators without manual driver intervention.

## 📌 Features
- **Real-Time GPS Tracking:** Uses NEO-6M GPS module to read heading angles and course data.
- **Microcontroller Processing:** ESP32 calculates directional changes and speed parameters.
- **Automated LED Signaling:** Triggers left/right indicators using non-blocking LED blinking logic.
- **Power Management:** Integrated 12V-to-5V DC-DC buck converter circuit.

## 🛠️ Hardware Components
- ESP32 Development Board
- NEO-6M GPS Module
- DC-DC Buck Converter (12V step-down)
- LED Indicators + 220Ω Resistors
- 12V Power Source / Battery

## 💻 Software & Libraries
- **Language:** C++ / Embedded C
- **IDE:** Arduino IDE
- **Libraries:** `TinyGPS++`, `HardwareSerial`, `WiFi.h`

## ⚙️ How It Works
1. GPS module streams NMEA data over UART serial communication.
2. ESP32 parses heading angles (`gps.course.deg()`) and speed (`gps.speed.kmph()`).
3. Heading differences exceeding 8 degrees trigger corresponding LED indicator sequences.
