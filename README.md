# IoT Programmable Load Shedding Controller

## Description
This project automates load shedding based on real-time AC current measurements, enabling intelligent power management. It combines a microcontroller (Arduino), IoT connectivity (ESP8266), and relays to disconnect/reconnect loads as needed. Features include AC voltage/current monitoring, relay control, an LCD display, and remote management via a web server.

## Features
- Automated load shedding based on current thresholds (0-1000 mA range)
- Real-time AC voltage/current measurement (using ACS712 sensor)
- 4 relay modules + 6 programmable GPIO-driven loads
- 20x4 LCD for local monitoring
- Wi-Fi integration for remote control (ESP8266)
- Server/client communication for load prioritization

## Pin Mapping
| Arduino Pin | Component                | Note                       |
|-------------|--------------------------|----------------------------|
| A0          | ACS712 Current Sensor    | Analog input               |
| A1          | AC Voltage Sensor        | Analog input (scaled 43.4x)|
| A2–A5       | Relay Control (RLY1-RLY4)| Digital outputs            |
| 0,1         | Load 5, Load 6           | Digital outputs            |
| 2–7         | LCD (RS, E, D4–D7)       | LiquidCrystal library       |
| 8–9         | ESP8266 (RX, TX)         | SoftwareSerial comms       |
| 10–13       | Load 1–Load 4            | Digital outputs            |

## Setup
1. **Hardware**:  
   - Connect ACS712 sensor to AC line (ensure isolation!)  
   - Wire relays to controlled loads  
   - Interface ESP8266 via SoftwareSerial (pins 8,9)  
2. **Firmware**:  
   - Flash `LoadSheding.ino` to Arduino (handles load control)  
   - Flash `ESP8266_Link.ino` to ESP8266 (Wi-Fi communication)  
3. **Configuration**:  
   - Adjust `CON
