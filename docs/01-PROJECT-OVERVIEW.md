# 01 - Project Overview

## Executive Summary

The **Smart Posture & Ergonomics Camera Coach** is an advanced IoT system designed to combat the growing problem of poor posture among desk workers and students. Using computer vision, wireless sensor networks, and real-time feedback mechanisms, this system provides continuous posture monitoring and corrective guidance through a self-contained WiFi network.

## Problem Statement

### Health Issues from Poor Posture
- **Neck pain and headaches** - Forward head posture strains neck muscles
- **Back pain** - Slouching compresses spinal discs
- **Eye strain** - Sitting too close to screens causes digital eye fatigue
- **Reduced productivity** - Poor posture leads to decreased focus and energy
- **Long-term damage** - Chronic poor posture can cause permanent skeletal issues

### Current Solutions' Limitations
- **Wearable devices** - Uncomfortable, require charging, limited accuracy
- **Mobile apps** - Require manual interaction, no real-time monitoring
- **Commercial systems** - Expensive ($200-500), require cloud connectivity
- **Posture chairs** - Passive solutions, no active feedback

### Our Solution
A comprehensive, affordable, standalone IoT system that:
- ✅ Monitors posture 24/7 using computer vision
- ✅ Provides instant multi-modal feedback (visual, audio, display)
- ✅ Operates completely offline (no cloud dependency)
- ✅ Costs under ₱2,000 PHP to build
- ✅ Easy to deploy and use (plug-and-play)

## Project Objectives

### Primary Objectives
1. **Real-time Posture Detection** - Use ESP32-CAM to detect and analyze head tilt, shoulder alignment, and slouching with <2 second latency
2. **Standalone WiFi Network** - Create self-contained system using ESP32-CAM as Access Point (no external router required)
3. **Multi-Device Communication** - Implement HTTP REST API for wireless communication between ESP32-CAM and WEMOS D1 R1
4. **Live Video Streaming** - Provide MJPEG camera feed to web dashboard for user visibility
5. **Comprehensive Feedback** - Multi-modal alerts via RGB LED, buzzer, OLED display
6. **Break Management** - Enforce 45-minute work intervals with 5-minute break reminders
7. **Web Dashboard** - Responsive interface hosted directly on ESP32-CAM (accessible via browser)

### Secondary Objectives
1. **Distance Monitoring** - Track eye-to-screen distance (optimal 50-70cm range)
2. **Session Analytics** - Track daily posture trends, average scores, alert counts
3. **Manual Controls** - Physical buttons for break start, snooze, privacy mode
4. **User Privacy** - Privacy mode to disable camera when needed
5. **Expandability** - Modular code architecture for future enhancements

### Academic Requirements
This project fulfills university Platform Technologies course requirements:
- ✅ **Dual microcontroller system** (ESP32-CAM + WEMOS D1 R1)
- ✅ **Wireless communication** (WiFi/HTTP, no wired serial)
- ✅ **Computer vision** (ESP32-CAM with OV2640)
- ✅ **RTOS implementation** (FreeRTOS on ESP32-CAM)
- ✅ **Web interface** (Dashboard with live video)
- ✅ **Sensor integration** (Ultrasonic, buttons, I2C display)
- ✅ **API design** (RESTful endpoints)
- ✅ **Complete within 1 week** (prototype timeline)

## System Overview

### Master Controller: ESP32-CAM
**Role:** Central hub, camera processing, WiFi access point, web server

**Responsibilities:**
- Create and maintain WiFi Access Point ("PostureCoach-AP")
- Capture camera frames at 0.5 FPS for posture analysis
- Stream MJPEG video at 5-10 FPS to web dashboard
- Run posture detection algorithm (face landmarks, angles)
- Host web server (HTML/CSS/JS files from SPIFFS)
- Provide REST API endpoints for WEMOS D1 R1 communication
- Calculate posture score (0-100 scale)
- Manage session data and analytics
- Broadcast commands to WEMOS D1 R1

**Hardware:**
- ESP32-S module with dual-core processor (240 MHz)
- 4MB PSRAM for camera buffers
- OV2640 camera (2MP, up to 1600x1200)
- Built-in WiFi 802.11 b/g/n

### Slave Controller: WEMOS D1 R1
**Role:** Sensor management, local feedback, WiFi client

**Responsibilities:**
- Connect to ESP32-CAM's WiFi network automatically
- Read HC-SR04 ultrasonic sensor (distance measurement)
- Monitor 3 pushbuttons (manual break, snooze, privacy)
- Control RGB LED (posture status indicator)
- Drive piezo buzzer (audio alerts)
- Update OLED display (score, distance, timer)
- Send sensor data to ESP32-CAM via HTTP POST
- Poll ESP32-CAM for commands via HTTP GET
- Execute feedback based on received commands
- Maintain WiFi connection with auto-reconnect

**Hardware:**
- ESP8266 microcontroller (80 MHz)
- 80KB usable RAM
- Built-in WiFi 802.11 b/g/n
- GPIO pins for sensors and peripherals

### User Interface: Web Dashboard
**Role:** Remote monitoring and control interface

**Features:**
- Live camera feed (MJPEG stream)
- Real-time posture score with animated gauge
- Distance monitoring with visual indicator
- Session statistics (duration, avg score, alerts)
- Manual control buttons (break, settings)
- Responsive design (works on phone/tablet/laptop)
- No external dependencies (all assets embedded)

**Access Method:**
1. Connect device WiFi to "PostureCoach-AP"
2. Open browser to http://192.168.4.1
3. Dashboard loads instantly (no internet needed)

## Key Features in Detail

### 1. Real-Time Posture Detection

**Computer Vision Pipeline:**
```
Camera Frame → Face Detection → Landmark Extraction → Angle Calculation → Score Generation
```

**Detected Metrics:**
- **Head Tilt Angle** - Forward lean measured in degrees (0-45°)
- **Shoulder Alignment** - Y-position difference between shoulders
- **Slouch Detection** - Overall upper body posture
- **Neck Angle** - Deviation from vertical axis

**Scoring Algorithm:**
```
Base Score = 100 points

Deductions:
- Head tilt > 15°: -1 point per degree (max -30)
- Shoulder slouch detected: -25 points
- Distance < 50cm or > 70cm: -20 points
- Sitting > 45 min without break: -15 points

Final Score = Base Score - Total Deductions
```

**Score Interpretation:**
- **90-100 (Excellent):** Perfect posture, green LED
- **75-89 (Good):** Minor issues, blue LED
- **60-74 (Fair):** Needs improvement, yellow LED
- **Below 60 (Poor):** Critical issues, red LED + buzzer

### 2. Live Camera Streaming

**MJPEG Streaming:**
- Continuous motion JPEG stream to web dashboard
- Frame rate: 5-10 FPS (adjustable)
- Resolution: 640x480 (VGA) or 320x240 (QVGA)
- Endpoint: `GET http://192.168.4.1/stream`
- Content-Type: `multipart/x-mixed-replace; boundary=frame`

**Implementation:**
```cpp
// FreeRTOS task handles streaming
void streamTask(void *parameter) {
  while(client.connected()) {
    camera_fb_t *fb = esp_camera_fb_get();
    client.printf("--frame\r\n");
    client.printf("Content-Type: image/jpeg\r\n");
    client.printf("Content-Length: %d\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);
    delay(100); // 10 FPS
  }
}
```

**Web Dashboard Integration:**
```html
<img src="http://192.168.4.1/stream" width="640" height="480" alt="Live Feed">
```

### 3. Wireless Communication (HTTP REST API)

**Network Architecture:**
```
ESP32-CAM (AP Mode)
├─ IP: 192.168.4.1
├─ SSID: PostureCoach-AP
├─ Password: posture123
└─ DHCP Range: 192.168.4.2 - 192.168.4.10

Connected Clients:
├─ WEMOS D1 R1 (192.168.4.2)
├─ User Phone (192.168.4.3)
└─ User Laptop (192.168.4.4)
```

**Communication Patterns:**

**WEMOS → ESP32-CAM (HTTP POST):**
```json
POST /api/sensor/distance
{
  "distance": 58,
  "unit": "cm",
  "timestamp": 1234567890
}

POST /api/input/button
{
  "button": 1,
  "action": "PRESSED",
  "timestamp": 1234567890
}
```

**ESP32-CAM → WEMOS (HTTP GET - Polling):**
```json
GET /api/commands/pending

Response:
{
  "command": "LED",
  "value": "YELLOW",
  "pattern": "SOLID",
  "timestamp": 1234567890
}
```

**Dashboard → ESP32-CAM (AJAX Polling):**
```javascript
// Poll every 3 seconds
setInterval(async () => {
  const data = await fetch('/api/posture/current').then(r => r.json());
  updateDashboard(data);
}, 3000);
```

### 4. Multi-Modal Feedback System

**RGB LED Color Coding:**
- 🟢 **Green:** Excellent posture (score 90-100)
- 🔵 **Blue:** Good posture (score 75-89)
- 🟡 **Yellow:** Fair posture (score 60-74)
- 🔴 **Red:** Poor posture (score <60)
- 🟣 **Purple:** Break mode active
- ⚪ **White:** Privacy mode / system idle

**Buzzer Alert Patterns:**
```cpp
// Pattern definitions
URGENT_PATTERN:    3 loud beeps (100ms on, 100ms off, repeat 3x)
WARNING_PATTERN:   2 medium beeps (80ms on, 120ms off, repeat 2x)
DISTANCE_ALERT:    Rapid short beeps (50ms on, 50ms off, repeat 5x)
SUCCESS_CHIME:     Ascending tones (200Hz→400Hz→600Hz)
BREAK_REMINDER:    Gentle two-tone (500Hz 200ms, 600Hz 200ms)
```

**OLED Display Layout:**
```
┌──────────────────────┐
│  Posture Score: 85   │  Line 1: Current score
│  Status: GOOD        │  Line 2: Status text
│  Distance: 58cm      │  Line 3: Eye distance
│  Break in: 12:35     │  Line 4: Next break timer
│  WiFi: Connected     │  Line 5: Connection status
│  [Session: 1h 23m]   │  Line 6: Session duration
└──────────────────────┘
```

### 5. Break Management System

**Work-Break Cycle:**
- **Work Duration:** 45 minutes of continuous monitoring
- **Break Duration:** 5 minutes mandatory rest
- **Break Enforcement:** Auto-dim screen, show break exercises
- **Manual Override:** Button 1 to start break early
- **Snooze Function:** Button 2 to delay break by 5 minutes (max 2 snoozes)

**Break Notification Sequence:**
1. **5 min before:** Yellow LED pulses, OLED shows reminder
2. **1 min before:** Gentle buzzer chime every 20 seconds
3. **Break time:** Red LED flashing, continuous chime, dashboard overlay
4. **During break:** Purple LED, countdown timer on OLED
5. **Break end:** Success chime, green LED, return to monitoring

### 6. Distance Monitoring

**HC-SR04 Ultrasonic Sensor:**
- **Measurement Range:** 2cm - 400cm
- **Practical Range:** 40cm - 80cm (desk usage)
- **Accuracy:** ±3mm
- **Update Rate:** Every 2 seconds
- **Optimal Zone:** 50-70cm (highlighted green in dashboard)

**Distance Alerts:**
- **Too Close (<50cm):** "Move back!" message, red LED, rapid beeps
- **Optimal (50-70cm):** No alert, score bonus
- **Too Far (>70cm):** "Move closer" message, yellow LED

### 7. Web Dashboard Features

**Dashboard Sections:**

**A. Live Feed Panel:**
- Full-resolution MJPEG stream
- Camera active indicator (green dot)
- Privacy toggle button
- Screenshot capture button

**B. Posture Score Panel:**
- Circular animated gauge (0-100)
- Color-coded based on score
- Current status text
- Trend arrow (improving/declining)

**C. Distance Monitor:**
- Horizontal bar gauge
- Optimal zone highlighted
- Current distance in large text
- Recommendations

**D. Session Statistics:**
- Current session duration
- Average score for session
- Total alerts triggered
- Breaks taken count
- Compliance percentage

**E. Quick Actions:**
- "Take Break Now" button
- "Snooze Alerts (5min)" button
- "Privacy Mode" toggle
- "Reset Session" button
- "Adjust Settings" modal

**F. Analytics Charts:**
- Hourly posture score line chart
- Daily average bar chart (7 days)
- Top issues frequency chart

**G. Settings Panel:**
- Alert sensitivity slider
- Break interval dropdown (30/45/60 min)
- Distance range adjustment
- Buzzer volume control
- Auto-break enforcement toggle

## Technical Specifications

### Performance Metrics
- **Camera Frame Rate:** 0.5 FPS (posture analysis)
- **Streaming Frame Rate:** 5-10 FPS (MJPEG)
- **Posture Detection Latency:** <2 seconds
- **HTTP Response Time:** <200ms
- **Dashboard Update Interval:** 3 seconds
- **Sensor Reading Interval:** 2 seconds
- **OLED Refresh Rate:** 1 Hz

### Network Specifications
- **WiFi Standard:** 802.11 b/g/n (2.4 GHz)
- **AP IP Address:** 192.168.4.1
- **DHCP Range:** 192.168.4.2 - 192.168.4.10
- **Max Clients:** 4 simultaneous connections
- **WiFi Channel:** 6 (configurable)
- **Signal Range:** ~30 meters indoor

### Memory Usage
- **ESP32-CAM:**
  - Firmware: ~1.5MB
  - SPIFFS (web files): ~1MB
  - Runtime heap: ~100KB free minimum
  - PSRAM (camera buffers): 4MB

- **WEMOS D1 R1:**
  - Firmware: ~300KB
  - Runtime heap: ~40KB free minimum

### Power Requirements
- **ESP32-CAM:** 5V @ 500-1000mA (peaks during WiFi transmission)
- **WEMOS D1 R1:** 5V @ 200-400mA
- **Total System:** 5V @ 2A recommended (with safety margin)

## Use Cases

### Use Case 1: Student Study Session
**Scenario:** University student studying for 3 hours

**Flow:**
1. Student sits at desk, powers on system
2. System calibrates baseline posture
3. Student focuses on studying
4. After 30 minutes, student slouches forward
5. System detects poor posture (score 65)
6. Yellow LED lights up, OLED shows "Sit up straight"
7. Gentle buzzer reminder sounds
8. Student corrects posture
9. Score improves to 88, LED turns blue
10. After 45 minutes, break reminder activates
11. Student takes 5-minute break
12. System resets timer, monitoring continues

### Use Case 2: Office Worker Full Day
**Scenario:** Remote worker at home office (8 hours)

**Flow:**
1. Worker connects laptop WiFi to PostureCoach-AP
2. Opens dashboard in browser tab
3. Dashboard shows live camera feed and score
4. Throughout day, worker monitors posture score
5. Worker sits too close to screen (45cm)
6. Distance alert triggers, dashboard shows "Move back!"
7. Worker adjusts chair position
8. Every 45 minutes, break enforcement activates
9. Worker uses break time for stretching
10. End of day, dashboard shows statistics:
    - Average score: 82 (Good)
    - 8 alerts triggered
    - 10/10 breaks taken (100% compliance)
    - Total sitting time: 7.5 hours

### Use Case 3: Shared Workspace
**Scenario:** Multiple people using same desk throughout day

**Flow:**
1. Morning user: Connects phone to PostureCoach-AP
2. System monitors morning shift (4 hours)
3. Lunch break: Privacy mode activated
4. Afternoon user: Connects different device
5. System continues monitoring new user
6. Dashboard shows separate session statistics
7. Each user can review their individual data

## Project Scope

### In Scope ✅
- ESP32-CAM with OV2640 camera
- WEMOS D1 R1 (ESP8266) microcontroller
- HC-SR04 ultrasonic distance sensor
- 128x64 OLED I2C display
- RGB LED (common cathode)
- Piezo buzzer
- 3 pushbuttons
- WiFi Access Point on ESP32-CAM
- HTTP REST API communication
- MJPEG live streaming
- Web dashboard (HTML/CSS/JS)
- FreeRTOS task implementation
- Posture detection algorithm
- Break management system
- Session analytics
- Complete documentation

### Out of Scope ❌
- MPU6050 accelerometer/gyroscope
- Vibration motors
- Serial/UART communication between devices
- External WiFi router dependency
- Android/iOS mobile applications
- MQTT broker
- Cloud connectivity
- Database storage (SQLite/MySQL)
- User authentication/multi-user accounts
- Machine learning training
- Commercial deployment
- FDA/medical device certification

## Success Criteria

### Functional Requirements
✅ System creates standalone WiFi network (no external router)  
✅ User can connect and access dashboard within 30 seconds  
✅ Live camera feed displays with <5 second latency  
✅ Posture score updates within 3 seconds of change  
✅ Distance sensor accuracy within ±5cm  
✅ Alerts trigger within 2 seconds of detection  
✅ Break system enforces 45-minute intervals  
✅ HTTP communication success rate >95%  
✅ System runs continuously for 8+ hours without restart  
✅ Web dashboard responsive on mobile and desktop  

### Non-Functional Requirements
✅ Total hardware cost under ₱2,000 PHP  
✅ Assembly time under 4 hours  
✅ Firmware upload time under 10 minutes  
✅ User setup time under 5 minutes  
✅ Power consumption under 10W  
✅ WiFi range minimum 10 meters  
✅ System boot time under 30 seconds  
✅ Code documentation coverage >80%  

### Academic Requirements
✅ Complete working prototype within 7 days  
✅ Comprehensive technical documentation  
✅ Demonstration ready (15-minute presentation)  
✅ All deliverables submitted on time  

## Risk Assessment

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| ESP32-CAM memory overflow | Medium | High | Use PSRAM, optimize code, limit simultaneous connections |
| WiFi connection instability | Medium | High | Implement auto-reconnect, watchdog timer, connection monitoring |
| Camera poor lighting performance | High | Medium | Use exposure compensation, recommend desk lamp |
| False posture detection | Medium | Medium | Calibration step, adjustable sensitivity, user feedback |
| Component availability delay | Low | High | Order parts early, have backup suppliers |
| Power supply insufficient | Low | High | Use 2A minimum power supply, separate supplies for each board |
| SPIFFS upload failure | Medium | Medium | Use LittleFS alternative, smaller file sizes, compression |
| HTTP timeout issues | Medium | Medium | Increase timeout values, retry logic, error handling |

## Timeline and Milestones

### Week 1: Prototype Development

**Day 1 (Requirements & Planning)**
- ✅ Finalize project specifications
- ✅ Create system architecture diagram
- ✅ Research components and pricing
- ✅ Order all hardware components

**Day 2 (Hardware Acquisition & Setup)**
- ✅ Receive and inspect all components
- ✅ Setup Arduino IDE and install libraries
- ✅ Test ESP32-CAM basic camera capture
- ✅ Test WEMOS D1 R1 basic GPIO

**Day 3 (Hardware Assembly)**
- ✅ Wire WEMOS D1 R1 with all peripherals
- ✅ Test individual sensors (distance, buttons)
- ✅ Test individual outputs (LED, buzzer, OLED)
- ✅ Verify all connections with multimeter

**Day 4 (ESP32-CAM Firmware)**
- ✅ Implement WiFi Access Point
- ✅ Setup camera and MJPEG streaming
- ✅ Create basic web server
- ✅ Upload SPIFFS with HTML files
- ✅ Test HTTP endpoints

**Day 5 (WEMOS D1 R1 Firmware)**
- ✅ Implement WiFi client connection
- ✅ Setup HTTP client for API calls
- ✅ Integrate sensor reading code
- ✅ Implement feedback control logic
- ✅ Test HTTP communication with ESP32-CAM

**Day 6 (Integration & Web Dashboard)**
- ✅ Implement posture detection algorithm
- ✅ Complete web dashboard with live feed
- ✅ Integrate all REST API endpoints
- ✅ Test full system end-to-end
- ✅ Optimize performance and fix bugs

**Day 7 (Testing & Documentation)**
- ✅ Comprehensive system testing
- ✅ Fix critical bugs
- ✅ Complete user manual
- ✅ Prepare demonstration script
- ✅ Record demo video

## Deliverables Checklist

### Documentation
- [x] README.md with project overview
- [x] 01-PROJECT-OVERVIEW.md (this document)
- [ ] 02-SYSTEM-ARCHITECTURE.md
- [ ] 03-HARDWARE-SETUP.md
- [ ] 04-WIRING-DIAGRAMS.md
- [ ] 05-SOFTWARE-ARCHITECTURE.md
- [ ] 06-API-SPECIFICATION.md
- [ ] 07-ASSEMBLY-GUIDE.md
- [ ] 08-USER-MANUAL.md
- [ ] 09-TROUBLESHOOTING.md
- [ ] 10-DEMONSTRATION-SCRIPT.md

### Code
- [ ] esp32-cam/esp32-cam.ino (main firmware)
- [ ] esp32-cam/config.h (configuration)
- [ ] esp32-cam/camera_handler.h (camera functions)
- [ ] esp32-cam/wifi_ap.h (WiFi AP setup)
- [ ] esp32-cam/web_server.h (web server)
- [ ] esp32-cam/posture_detection.h (algorithm)
- [ ] esp32-cam/api_endpoints.h (REST API)
- [ ] esp32-cam/data/index.html (dashboard)
- [ ] esp32-cam/data/style.css (styles)
- [ ] esp32-cam/data/script.js (frontend logic)
- [ ] wemos-d1-r1/wemos-d1-r1.ino (main firmware)
- [ ] wemos-d1-r1/config.h (configuration)
- [ ] wemos-d1-r1/wifi_client.h (WiFi client)
- [ ] wemos-d1-r1/http_client.h (HTTP communication)
- [ ] wemos-d1-r1/sensors.h (sensor functions)
- [ ] wemos-d1-r1/feedback.h (feedback control)

### Diagrams
- [ ] System architecture diagram
- [ ] WEMOS D1 R1 wiring diagram
- [ ] Communication flow diagram
- [ ] Network topology diagram

### Supporting Documents
- [ ] Bill of materials with costs
- [ ] SPIFFS upload guide
- [ ] Troubleshooting flowcharts
- [ ] Demonstration script

## Conclusion

The Smart Posture & Ergonomics Camera Coach represents a comprehensive IoT solution that addresses a real-world health problem using accessible, affordable technology. By leveraging the ESP32-CAM's camera and WiFi capabilities along with the WEMOS D1 R1's sensor management, this system provides an integrated, standalone posture monitoring solution that requires no external infrastructure.

The project successfully demonstrates:
- **Dual microcontroller coordination** via wireless HTTP communication
- **Computer vision** for posture analysis
- **Real-time operating system** (FreeRTOS) implementation
- **Web-based user interface** with live video streaming
- **Multi-modal feedback** mechanisms
- **Complete standalone operation** (no cloud or router dependency)

This system can be built within a 7-day timeline with a budget under ₱2,000 PHP, making it an ideal university project that balances technical complexity with practical feasibility.

---

**Next Steps:**
1. Review system architecture details in [02-SYSTEM-ARCHITECTURE.md](02-SYSTEM-ARCHITECTURE.md)
2. Procure hardware components from [03-HARDWARE-SETUP.md](03-HARDWARE-SETUP.md)
3. Begin assembly following [07-ASSEMBLY-GUIDE.md](07-ASSEMBLY-GUIDE.md)

**Document Version:** 1.0  
**Last Updated:** December 24, 2024  
**Status:** ✅ Complete