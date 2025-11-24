# Smart Posture & Ergonomics Camera Coach IoT

![Project Status](https://img.shields.io/badge/status-in%20development-yellow)
![Platform](https://img.shields.io/badge/platform-ESP32%20%7C%20ESP8266-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## 🎯 Project Overview

The **Smart Posture & Ergonomics Camera Coach** is a comprehensive IoT solution designed for university-level Platform Technologies coursework. This system uses computer vision and wireless sensor networks to monitor and improve user posture in real-time through a standalone WiFi network.

### Key Features

✅ **Standalone WiFi Network** - ESP32-CAM creates its own Access Point (no external router needed)  
✅ **Live Camera Feed** - MJPEG streaming directly to web dashboard  
✅ **Real-time Posture Analysis** - Computer vision-based posture scoring (0-100)  
✅ **Multi-modal Feedback** - Visual (RGB LED), Audio (Buzzer), Display (OLED)  
✅ **Web Dashboard** - Responsive interface hosted on ESP32-CAM  
✅ **Wireless Communication** - HTTP REST API between ESP32-CAM and WEMOS D1 R1  
✅ **Break Management** - 45-minute work intervals with enforced breaks  
✅ **Distance Monitoring** - Ultrasonic sensor tracks eye-to-screen distance

## 🏗️ System Architecture

```
                    ┌─────────────────────┐
                    │   ESP32-CAM         │
                    │   WiFi AP Mode      │
                    │   SSID: PostureCoach-AP
                    │   IP: 192.168.4.1   │
                    │   • Camera          │
                    │   • Web Server      │
                    │   • REST API        │
                    │   • MJPEG Stream    │
                    └──────────┬──────────┘
                               │ WiFi
        ┌──────────────────────┼──────────────────────┐
        │                      │                      │
   ┌────▼──────┐         ┌─────▼──────┐       ┌──────▼─────┐
   │ WEMOS D1  │         │ User Phone │       │   Laptop   │
   │    R1     │         │  Browser   │       │  Browser   │
   │ (Client)  │         │192.168.4.1 │       │192.168.4.1 │
   └───────────┘         └────────────┘       └────────────┘
        │
   [Sensors]
   • Distance
   • Buttons
   • Buzzer
   • RGB LED
   • OLED
```

## 🔧 Hardware Components

| Component | Quantity | Purpose | Est. Cost (PHP) |
|-----------|----------|---------|----------------|
| ESP32-CAM (with OV2640) | 1 | Master controller, camera, WiFi AP | ₱400-500 |
| WEMOS D1 R1 (ESP8266) | 1 | Slave controller, sensor management | ₱250-350 |
| HC-SR04 Ultrasonic Sensor | 1 | Distance measurement | ₱50-80 |
| 128x64 OLED Display (I2C) | 1 | Local feedback display | ₱150-200 |
| RGB LED (Common Cathode) | 1 | Visual status indicator | ₱10-20 |
| Piezo Buzzer | 1 | Audio alerts | ₱15-30 |
| Pushbuttons | 3 | Manual controls | ₱15-30 |
| 220Ω Resistors | 3 | LED current limiting | ₱5-10 |
| Jumper Wires | 30 | Connections | ₱50-100 |
| Breadboard (830 points) | 2 | Prototyping | ₱100-150 |
| 5V 2A Power Supply/USB | 2 | Power for both boards | ₱200-300 |

**Total Estimated Cost:** ₱1,245 - ₱1,770 PHP

## 📚 Documentation

### Getting Started
1. [Project Overview](docs/01-PROJECT-OVERVIEW.md) - Detailed project description and objectives
2. [System Architecture](docs/02-SYSTEM-ARCHITECTURE.md) - Technical architecture and design patterns
3. [Hardware Setup](docs/03-HARDWARE-SETUP.md) - Complete bill of materials and specifications

### Implementation
4. [Wiring Diagrams](docs/04-WIRING-DIAGRAMS.md) - Pin mappings and circuit diagrams
5. [Software Architecture](docs/05-SOFTWARE-ARCHITECTURE.md) - Code structure and FreeRTOS implementation
6. [API Specification](docs/06-API-SPECIFICATION.md) - REST API endpoints and communication protocol

### Deployment
7. [Assembly Guide](docs/07-ASSEMBLY-GUIDE.md) - Step-by-step assembly instructions
8. [User Manual](docs/08-USER-MANUAL.md) - How to connect and use the system
9. [Troubleshooting](docs/09-TROUBLESHOOTING.md) - Common issues and solutions
10. [Demonstration Script](docs/10-DEMONSTRATION-SCRIPT.md) - 15-minute project demo guide

## ⚡ Quick Start

### Prerequisites
- Arduino IDE 1.8.19 or later
- ESP32 board support package
- ESP8266 board support package
- Required Arduino libraries (see below)

### Arduino Libraries Required

**For ESP32-CAM:**
```
- ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
- AsyncTCP (https://github.com/me-no-dev/AsyncTCP)
- ArduinoJson v6.21.0+
- esp32-camera (included with ESP32 board package)
```

**For WEMOS D1 R1:**
```
- ESP8266WiFi (included with ESP8266 board package)
- ESP8266HTTPClient
- ArduinoJson v6.21.0+
- Adafruit_SSD1306
- Adafruit_GFX
- NewPing (for HC-SR04)
```

### Installation Steps

#### 1. Clone Repository
```bash
git clone https://github.com/hyoono/posture-coach-iot.git
cd posture-coach-iot
```

#### 2. Setup ESP32-CAM
```bash
# Install ESP32 board support in Arduino IDE
# File > Preferences > Additional Board Manager URLs:
# https://dl.espressif.com/dl/package_esp32_index.json

# Install required libraries via Library Manager

# Upload SPIFFS filesystem:
# 1. Place web files in esp32-cam/data/ folder
# 2. Tools > ESP32 Sketch Data Upload
# 3. Wait for upload completion (may take 2-3 minutes)

# Upload firmware:
# 1. Open esp32-cam/esp32-cam.ino
# 2. Select Board: "AI Thinker ESP32-CAM"
# 3. Set Partition Scheme: "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
# 4. Upload (connect GPIO0 to GND for programming mode)
```

#### 3. Setup WEMOS D1 R1
```bash
# Install ESP8266 board support in Arduino IDE
# File > Preferences > Additional Board Manager URLs:
# http://arduino.esp8266.com/stable/package_esp8266com_index.json

# Upload firmware:
# 1. Open wemos-d1-r1/wemos-d1-r1.ino
# 2. Select Board: "LOLIN(WEMOS) D1 R1"
# 3. Set Upload Speed: 115200
# 4. Upload via USB
```

#### 4. Hardware Assembly
```bash
# Follow wiring diagram in docs/04-WIRING-DIAGRAMS.md
# Connect all sensors and peripherals to WEMOS D1 R1
# Ensure proper power supply (5V 2A minimum for ESP32-CAM)
```

#### 5. Power On and Connect
```bash
# 1. Power on ESP32-CAM (creates "PostureCoach-AP" WiFi network)
# 2. Power on WEMOS D1 R1 (auto-connects to ESP32-CAM)
# 3. Connect your phone/laptop WiFi to "PostureCoach-AP"
# 4. Open browser and navigate to: http://192.168.4.1
# 5. View live camera feed and posture data
```

## 📁 Project Structure

```
posture-coach-iot/
├── README.md
├── LICENSE
├── .gitignore
├── docs/                           # Documentation
│   ├── 01-PROJECT-OVERVIEW.md
│   ├── 02-SYSTEM-ARCHITECTURE.md
│   ├── 03-HARDWARE-SETUP.md
│   ├── 04-WIRING-DIAGRAMS.md
│   ├── 05-SOFTWARE-ARCHITECTURE.md
│   ├── 06-API-SPECIFICATION.md
│   ├── 07-ASSEMBLY-GUIDE.md
│   ├── 08-USER-MANUAL.md
│   ├── 09-TROUBLESHOOTING.md
│   └── 10-DEMONSTRATION-SCRIPT.md
├── esp32-cam/                      # ESP32-CAM firmware
│   ├── esp32-cam.ino              # Main sketch
│   ├── config.h                   # Configuration
│   ├── camera_handler.h           # Camera functions
│   ├── wifi_ap.h                  # WiFi AP setup
│   ├── web_server.h               # Web server
│   ├── posture_detection.h        # Posture algorithm
│   ├── api_endpoints.h            # REST API handlers
│   └── data/                      # Web files (SPIFFS)
│       ├── index.html
│       ├── style.css
│       └── script.js
├── wemos-d1-r1/                    # WEMOS D1 R1 firmware
│   ├── wemos-d1-r1.ino            # Main sketch
│   ├── config.h                   # Configuration
│   ├── wifi_client.h              # WiFi client
│   ├── http_client.h              # HTTP communication
│   ├── sensors.h                  # Sensor handling
│   └── feedback.h                 # Feedback control
├── diagrams/                       # Visual diagrams
│   ├── system-architecture.png
│   ├── wiring-wemos-d1-r1.png
│   └── communication-flow.png
└── bill-of-materials.md           # BOM with costs
```

## 🔄 Communication Flow

### ESP32-CAM → WEMOS D1 R1
```
1. WEMOS polls: GET http://192.168.4.1/api/commands/pending
2. ESP32-CAM responds with commands (LED color, buzzer pattern, OLED message)
3. WEMOS executes feedback based on commands
```

### WEMOS D1 R1 → ESP32-CAM
```
1. WEMOS reads sensors (distance, buttons)
2. WEMOS sends: POST http://192.168.4.1/api/sensor/distance
3. ESP32-CAM updates posture score based on distance data
```

### User Device → ESP32-CAM
```
1. User connects to "PostureCoach-AP" WiFi network
2. User opens http://192.168.4.1 in browser
3. Dashboard loads with live camera feed from /stream endpoint
4. JavaScript polls /api/posture/current every 3 seconds for updates
```

## 🎨 Posture Scoring Algorithm

```
Score = 100 - (head_tilt_penalty + shoulder_penalty + distance_penalty + time_penalty)

• Head Tilt Penalty (0-30 points):
  - Forward tilt > 15°: penalty = angle - 15
  - Max penalty at 45° tilt

• Shoulder Penalty (0-25 points):
  - Slouch detected: penalty = 25
  - Slight slouch: penalty = 15

• Distance Penalty (0-20 points):
  - < 50cm: penalty = (50 - distance) / 2
  - > 70cm: penalty = (distance - 70) / 3

• Time Penalty (0-15 points):
  - > 45 min without break: penalty = 15
  - 30-45 min: penalty = 10

Score Interpretation:
90-100: Excellent (Green LED)
75-89:  Good (Blue LED)
60-74:  Fair (Yellow LED)
<60:    Poor (Red LED + Buzzer)
```

## 📊 7-Day Project Timeline

| Day | Tasks | Deliverables |
|-----|-------|--------------|
| **Day 1** | Requirements analysis, component research | Project specification document |
| **Day 2** | Hardware procurement, tool setup | All components acquired, Arduino IDE configured |
| **Day 3** | Wiring assembly, basic testing | Assembled hardware, tested connections |
| **Day 4** | ESP32-CAM firmware development | Working camera, WiFi AP, basic web server |
| **Day 5** | WEMOS D1 R1 firmware, sensor integration | Sensor readings, HTTP communication working |
| **Day 6** | Web dashboard, posture detection algorithm | Complete system integration |
| **Day 7** | Testing, debugging, documentation | Final demo-ready prototype |

## 🧪 Testing Checklist

- [ ] ESP32-CAM creates "PostureCoach-AP" WiFi network
- [ ] WEMOS D1 R1 connects to ESP32-CAM automatically
- [ ] User device can connect to PostureCoach-AP
- [ ] Web dashboard loads at http://192.168.4.1
- [ ] Live camera feed displays in dashboard (MJPEG stream)
- [ ] Posture score updates in real-time
- [ ] Distance sensor reads correctly (40-80cm range)
- [ ] RGB LED changes color based on posture score
- [ ] Buzzer sounds alerts for poor posture
- [ ] OLED displays current score and distance
- [ ] Manual break button triggers break mode
- [ ] Snooze button silences alerts for 5 minutes
- [ ] Privacy button stops camera feed
- [ ] HTTP communication stable between devices
- [ ] Break timer counts down correctly (5 minutes)
- [ ] Dashboard responsive on phone/tablet/laptop

## 🐛 Common Issues & Solutions

### ESP32-CAM Won't Upload
- **Solution:** Connect GPIO0 to GND before powering on, disconnect after upload

### WiFi AP Not Appearing
- **Solution:** Check ESP32-CAM power supply (needs 5V 2A), verify firmware uploaded correctly

### Camera Feed Black/Not Working
- **Solution:** Check camera ribbon cable connection, ensure proper seating

### WEMOS Won't Connect to ESP32-CAM
- **Solution:** Verify SSID/password in config.h, check ESP32-CAM is in AP mode

### Dashboard Not Loading
- **Solution:** Ensure SPIFFS uploaded correctly, check serial monitor for errors

See [docs/09-TROUBLESHOOTING.md](docs/09-TROUBLESHOOTING.md) for complete troubleshooting guide.

## 📖 Usage Instructions

### First-Time Setup
1. Power on ESP32-CAM (wait 10 seconds for AP to start)
2. Power on WEMOS D1 R1 (wait 5 seconds for connection)
3. On your phone/laptop, go to WiFi settings
4. Connect to network: **PostureCoach-AP**
5. Password: **posture123**
6. Open browser, navigate to: **http://192.168.4.1**
7. Adjust camera angle to capture your face and shoulders
8. Click "Calibrate" to set baseline posture

### Daily Usage
1. Sit at your desk with proper posture
2. System automatically monitors and alerts
3. Take breaks when prompted (every 45 minutes)
4. Check dashboard for daily statistics

### Manual Controls
- **Button 1 (D3):** Start break immediately
- **Button 2 (D5):** Snooze alerts for 5 minutes
- **Button 3 (D6):** Toggle privacy mode (camera off)

## 🎓 Academic Compliance

This project meets the following university course requirements:

✅ **Dual Microcontroller System:** ESP32-CAM (master) + WEMOS D1 R1 (slave)  
✅ **Wireless Communication:** HTTP REST API over WiFi (no wired UART)  
✅ **Computer Vision:** ESP32-CAM with OV2640 camera module  
✅ **Real-Time Operating System:** FreeRTOS on ESP32-CAM  
✅ **Web Interface:** Dashboard hosted on ESP32-CAM with live video  
✅ **Sensor Integration:** Distance, buttons, I2C display  
✅ **API Design:** RESTful endpoints with JSON responses  
✅ **Standalone System:** Self-contained WiFi network (no external infrastructure)

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Contributors

- **Your Name** - Initial work - [hyoono](https://github.com/hyoono)

## 🙏 Acknowledgments

- University Platform Technologies Course
- ESP32 and ESP8266 communities
- Arduino open-source ecosystem

## 📧 Contact

For questions or support, please open an issue in this repository.

---

**Project Status:** ✨ Ready for implementation (Week-long prototype timeline)  
**Last Updated:** December 2024  
**Version:** 1.0.0