# Wiring Diagrams and Pin Connections

## 🔌 Complete Hardware Setup Guide

This document provides detailed wiring instructions for connecting all components to the WEMOS D1 R1 and ESP32-CAM boards.

---

## WEMOS D1 R1 Pin Mapping Reference

### CRITICAL: Serial Monitor Compatibility
**TX (D1) and RX (D0) pins are RESERVED for Serial Monitor debugging and MUST remain free!**

### Complete Pin Table (Based on Board Silkscreen)

| Pin Label | GPIO | Function in Project | Component Connected |
|-----------|------|---------------------|---------------------|
| **RX<-D0** | GPIO3 | **RESERVED** | Serial RX - DO NOT USE |
| **TX->D1** | GPIO1 | **RESERVED** | Serial TX - DO NOT USE |
| **D2** | GPIO4 | Digital Output | HC-SR04 Trigger |
| **D3** (D15/SCL) | GPIO0 | I2C Clock | OLED SCL |
| **D4** (D14/SDA) | GPIO2 | I2C Data | OLED SDA |
| **D5** (D13/SCK) | GPIO14 | Digital Input (Pull-up) | Button 1 - Manual Break |
| **D6** (D12/MISO) | GPIO12 | Digital Input (Pull-up) | Button 2 - Snooze |
| **D7** (D11/MOSI) | GPIO13 | Digital Input (Pull-up) | Button 3 - Privacy |
| **D8** | GPIO15 | PWM Output | RGB LED - Red |
| **D9** (TX1) | GPIO3* | PWM Output | RGB LED - Green |
| **D10** (SS) | GPIO1* | PWM Output | RGB LED - Blue |
| **D13** (SCK header) | GPIO14 | Digital Input | HC-SR04 Echo |
| **A0** | ADC0 | Digital Output | Buzzer |
| **LED_BUILTIN** | GPIO2 | Digital Output | WiFi Status Indicator |
| **3V3** | - | Power Output | Not used (sensors use 5V) |
| **5V** | - | Power Output | HC-SR04, OLED power |
| **GND** | - | Ground | Common ground for all |

*Note: D9 and D10 use alternate GPIO mappings on some WEMOS D1 R1 boards

---

## Component Wiring Instructions

### 1. HC-SR04 Ultrasonic Distance Sensor

```
HC-SR04                  WEMOS D1 R1
--------                  -----------
VCC (Red)      --------> 5V
GND (Black)    --------> GND
TRIG (Orange)  --------> D2 (GPIO4)
ECHO (Yellow)  --------> D13 (GPIO14, header side SCK)
```

**Notes:**
- HC-SR04 requires 5V power for reliable operation
- Maximum range: 4 meters
- Minimum range: 2 cm
- Measure eye-to-screen distance (optimal: 50-70cm)

**Arduino Code:**
```cpp
#define TRIG_PIN D2
#define ECHO_PIN D13

pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);
```

---

### 2. 128x64 OLED Display (I2C)

```
OLED Display           WEMOS D1 R1
------------           -----------
VCC          --------> 5V
GND          --------> GND
SDA          --------> D4 (GPIO2, D14/SDA on silkscreen)
SCL          --------> D3 (GPIO0, D15/SCL on silkscreen)
```

**Notes:**
- Use I2C interface (0x3C address)
- Initialize with: `Wire.begin(D4, D3);` or `Wire.begin(OLED_SDA, OLED_SCL);`
- Display resolution: 128x64 pixels
- Shows posture score, distance, and status

**Arduino Code:**
```cpp
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA D4  // D14/SDA on silkscreen
#define OLED_SCL D3  // D15/SCL on silkscreen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
}
```

---

### 3. RGB LED (Common Cathode)

```
RGB LED              220Ω Resistor      WEMOS D1 R1
-------              -------------      -----------
Red Anode   -------> [220Ω Resistor] -> D8 (GPIO15)
Green Anode -------> [220Ω Resistor] -> D9 (GPIO3/TX1)
Blue Anode  -------> [220Ω Resistor] -> D10 (GPIO1/SS)
Common Cathode ----> (Direct connection) -> GND
```

**Notes:**
- **Common Cathode**: Connect cathode pin to GND
- Use 220Ω current-limiting resistors for each color
- PWM control for brightness (0-255)
- Color codes:
  - Green: Excellent posture (90-100)
  - Blue: Good posture (75-89)
  - Yellow: Fair posture (60-74)
  - Red: Poor posture (<60)

**Arduino Code:**
```cpp
#define LED_RED D8
#define LED_GREEN D9
#define LED_BLUE D10

pinMode(LED_RED, OUTPUT);
pinMode(LED_GREEN, OUTPUT);
pinMode(LED_BLUE, OUTPUT);

// Set color (Common Cathode: HIGH = ON)
analogWrite(LED_RED, 255);    // Red ON
analogWrite(LED_GREEN, 0);    // Green OFF
analogWrite(LED_BLUE, 0);     // Blue OFF
```

---

### 4. Piezo Buzzer

```
Buzzer               WEMOS D1 R1
------               -----------
Positive (+) ------> A0 (used as digital output)
Negative (-) ------> GND
```

**Notes:**
- A0 pin can be used as digital output for buzzer
- Use `tone()` function for different frequencies
- Buzzer patterns:
  - Success: 800Hz → 1000Hz → 1200Hz
  - Warning: 1000Hz (2 beeps)
  - Urgent: 1200Hz → 1400Hz → 1600Hz
  - Distance Alert: 1800Hz (5 rapid beeps)
  - Break Reminder: 800Hz → 600Hz

**Arduino Code:**
```cpp
#define BUZZER_PIN A0

pinMode(BUZZER_PIN, OUTPUT);

// Play tone
tone(BUZZER_PIN, 1000, 200);  // 1000Hz for 200ms
delay(250);
tone(BUZZER_PIN, 1000, 200);
```

---

### 5. Pushbuttons (with Internal Pull-up)

```
Button 1 (Manual Break)
-----------------------
One side  --------> D5 (GPIO14)
Other side -------> GND

Button 2 (Snooze)
-----------------
One side  --------> D6 (GPIO12)
Other side -------> GND

Button 3 (Privacy Mode)
-----------------------
One side  --------> D7 (GPIO13)
Other side -------> GND
```

**Notes:**
- **NO external pull-up resistors needed** - use internal pull-ups
- Button pressed = LOW (0)
- Button released = HIGH (1)
- Debounce delay: 50ms in software

**Arduino Code:**
```cpp
#define BUTTON1_PIN D5  // Manual Break
#define BUTTON2_PIN D6  // Snooze
#define BUTTON3_PIN D7  // Privacy

// Enable internal pull-up resistors
pinMode(BUTTON1_PIN, INPUT_PULLUP);
pinMode(BUTTON2_PIN, INPUT_PULLUP);
pinMode(BUTTON3_PIN, INPUT_PULLUP);

// Read button (Active LOW)
if (digitalRead(BUTTON1_PIN) == LOW) {
  // Button pressed
}
```

---

### 6. Power Supply

```
Power Supply         WEMOS D1 R1 + Components
------------         -------------------------
5V Output   -------> 5V pin (powers board and sensors)
GND         -------> GND pin
```

**Requirements:**
- **WEMOS D1 R1**: 5V 500mA minimum
- **All components**: Total ~800mA
- **Recommended**: 5V 2A USB power supply or wall adapter
- Can power via USB or external 5V supply

---

## ESP32-CAM Pin Configuration

### Camera Module Connections (OV2640)

The ESP32-CAM module comes with the OV2640 camera pre-connected. **Do not disconnect the camera ribbon cable.**

### ESP32-CAM Pin Assignment (AI-Thinker Model)

| Pin Name | GPIO | Function |
|----------|------|----------|
| PWDN | GPIO32 | Power Down |
| RESET | -1 | Reset (not used) |
| XCLK | GPIO0 | Clock |
| SIOD | GPIO26 | I2C Data |
| SIOC | GPIO27 | I2C Clock |
| Y9 | GPIO35 | Data Bit 9 |
| Y8 | GPIO34 | Data Bit 8 |
| Y7 | GPIO39 | Data Bit 7 |
| Y6 | GPIO36 | Data Bit 6 |
| Y5 | GPIO21 | Data Bit 5 |
| Y4 | GPIO19 | Data Bit 4 |
| Y3 | GPIO18 | Data Bit 3 |
| Y2 | GPIO5 | Data Bit 2 |
| VSYNC | GPIO25 | Vertical Sync |
| HREF | GPIO23 | Horizontal Reference |
| PCLK | GPIO22 | Pixel Clock |

### ESP32-CAM Power Requirements

```
Power Supply         ESP32-CAM Module
------------         ----------------
5V Output   -------> 5V pin
GND         -------> GND pin
```

**Important Notes:**
- ESP32-CAM requires stable 5V 2A power supply
- Camera operation draws significant current (up to 500mA peak)
- **Do NOT power from computer USB during camera streaming**
- Use external power supply or powered USB hub
- Add 470µF capacitor between 5V and GND for stability (optional but recommended)

### Programming ESP32-CAM

To upload firmware, connect:
1. **GPIO0 to GND** (programming mode)
2. Connect FTDI programmer:
   - FTDI TX → ESP32 RX (GPIO3)
   - FTDI RX → ESP32 TX (GPIO1)
   - FTDI 5V → ESP32 5V
   - FTDI GND → ESP32 GND
3. Press RESET button
4. Upload code via Arduino IDE
5. **Disconnect GPIO0 from GND**
6. Press RESET to run

---

## Complete System Wiring Diagram (ASCII Art)

```
═══════════════════════════════════════════════════════════════════════════════
                        WEMOS D1 R1 CONNECTIONS
═══════════════════════════════════════════════════════════════════════════════

                            ┌──────────────────┐
                            │   WEMOS D1 R1    │
                            │    ESP8266       │
                            │                  │
                      RX◄─D0│ ═════════════    │ (Reserved for Serial)
                      TX►─D1│ ═════════════    │ (Reserved for Serial)
                            │                  │
    HC-SR04 TRIG ───────D2│  GPIO4           │
    OLED SCL     ───────D3│  GPIO0 (D15/SCL) │
    OLED SDA     ───────D4│  GPIO2 (D14/SDA) │
    Button 1     ───────D5│  GPIO14          │──── Manual Break
    Button 2     ───────D6│  GPIO12          │──── Snooze Alerts
    Button 3     ───────D7│  GPIO13          │──── Privacy Toggle
    RGB Red      ───[220Ω]─D8│  GPIO15          │
    RGB Green    ───[220Ω]─D9│  TX1             │
    RGB Blue     ───[220Ω]─D10│ SS              │
                            │                  │
                            │              D13│──── HC-SR04 ECHO
                            │                  │
                            │              A0 │──── Buzzer
                            │                  │
                            │              5V │◄─── Power In (5V 2A)
                            │             GND │◄─── Common Ground
                            │             3V3 │ (not used)
                            └──────────────────┘

═══════════════════════════════════════════════════════════════════════════════
                          COMPONENT CONNECTIONS
═══════════════════════════════════════════════════════════════════════════════

┌─────────────────┐          ┌─────────────────┐          ┌─────────────────┐
│   HC-SR04       │          │  OLED 128x64    │          │   RGB LED       │
│  Ultrasonic     │          │    I2C          │          │ Common Cathode  │
├─────────────────┤          ├─────────────────┤          ├─────────────────┤
│ VCC  → 5V       │          │ VCC  → 5V       │          │ R → [220Ω] → D8│
│ GND  → GND      │          │ GND  → GND      │          │ G → [220Ω] → D9│
│ TRIG → D2       │          │ SDA  → D4       │          │ B → [220Ω] → D10│
│ ECHO → D13      │          │ SCL  → D3       │          │ Cathode → GND   │
└─────────────────┘          └─────────────────┘          └─────────────────┘

┌─────────────────┐          ┌─────────────────┐          ┌─────────────────┐
│  Pushbuttons    │          │ Piezo Buzzer    │          │  Power Supply   │
│  (3 buttons)    │          │                 │          │   5V 2A         │
├─────────────────┤          ├─────────────────┤          ├─────────────────┤
│ Btn1 → D5 + GND │          │ (+) → A0        │          │ 5V  → WEMOS 5V  │
│ Btn2 → D6 + GND │          │ (-) → GND       │          │ GND → WEMOS GND │
│ Btn3 → D7 + GND │          │                 │          │                 │
│ (Internal PU)   │          │                 │          │ 5V  → ESP32 5V  │
└─────────────────┘          └─────────────────┘          │ GND → ESP32 GND │
                                                           └─────────────────┘

═══════════════════════════════════════════════════════════════════════════════
                        ESP32-CAM CONFIGURATION
═══════════════════════════════════════════════════════════════════════════════

                            ┌──────────────────┐
                            │   ESP32-CAM      │
                            │   AI-Thinker     │
                            │                  │
                            │   [OV2640]       │ ◄── Camera Module (pre-connected)
                            │   Camera         │
                            │                  │
                            │  WiFi AP Mode    │
                            │  SSID: PostureCoach-AP
                            │  IP: 192.168.4.1 │
                            │                  │
        Power (5V 2A) ────► │ 5V               │
        Common Ground ────► │ GND              │
                            │                  │
                            │ GPIO0 ═══════════│ (To GND for programming)
                            └──────────────────┘

═══════════════════════════════════════════════════════════════════════════════
                          WIRELESS COMMUNICATION
═══════════════════════════════════════════════════════════════════════════════

                    ┌─────────────────────┐
                    │   ESP32-CAM         │
                    │   WiFi AP Mode      │
                    │   192.168.4.1       │
                    │                     │
                    │ • Camera Stream     │
                    │ • Web Server        │
                    │ • REST API          │
                    └──────────┬──────────┘
                               │
                        WiFi Connection
                        (HTTP Protocol)
                               │
        ┌──────────────────────┼──────────────────────┐
        │                      │                      │
   ┌────▼──────┐         ┌─────▼──────┐       ┌──────▼─────┐
   │ WEMOS D1  │         │ User Phone │       │   Laptop   │
   │    R1     │         │            │       │            │
   │ (Client)  │         │  Browser:  │       │  Browser:  │
   │           │         │192.168.4.1 │       │192.168.4.1 │
   └───────────┘         └────────────┘       └────────────┘
        │
   [All Sensors]
   - Distance
   - Buttons
   - RGB LED
   - Buzzer
   - OLED

═══════════════════════════════════════════════════════════════════════════════
```

---

## Breadboard Layout Recommendations

### Component Placement Tips

1. **WEMOS D1 R1**: Place at center of breadboard
2. **ESP32-CAM**: Use separate breadboard or power rail (high current draw)
3. **HC-SR04**: Mount on front of setup facing user
4. **OLED Display**: Mount for easy viewing
5. **RGB LED**: Position for visibility
6. **Buttons**: Place for easy access
7. **Buzzer**: Mount away from microphones (can cause feedback)

### Wire Color Coding (Recommended)

| Color | Purpose |
|-------|---------|
| Red | 5V Power |
| Black | Ground (GND) |
| Yellow/Orange | Signal/Data |
| Blue | I2C SCL |
| Green | I2C SDA |
| White | Digital I/O |
| Purple | PWM/Analog |

---

## Common Wiring Mistakes to Avoid

❌ **MISTAKE 1**: Connecting HC-SR04 to 3.3V (needs 5V)
✅ **FIX**: Use 5V pin for HC-SR04 power

❌ **MISTAKE 2**: Forgetting pull-up resistors for buttons
✅ **FIX**: Enable internal pull-ups with `INPUT_PULLUP`

❌ **MISTAKE 3**: Using TX/RX pins for peripherals
✅ **FIX**: Keep TX (D1) and RX (D0) free for Serial Monitor

❌ **MISTAKE 4**: Powering ESP32-CAM from computer USB
✅ **FIX**: Use external 5V 2A power supply

❌ **MISTAKE 5**: Wrong I2C pin assignment for OLED
✅ **FIX**: Use `Wire.begin(D4, D3)` for SDA=D4, SCL=D3

❌ **MISTAKE 6**: No current-limiting resistors for RGB LED
✅ **FIX**: Use 220Ω resistors on each color pin

❌ **MISTAKE 7**: Reversing RGB LED polarity
✅ **FIX**: Common Cathode → Connect cathode to GND

❌ **MISTAKE 8**: Weak power supply
✅ **FIX**: Use minimum 5V 2A supply for both boards

---

## Testing Each Component

### Step-by-Step Testing Procedure

1. **Power Test**
   - Connect power supply
   - Check voltage with multimeter (should be 5V ±5%)
   - Verify WEMOS D1 R1 LED lights up

2. **Serial Monitor Test**
   - Upload basic sketch with `Serial.begin(115200)`
   - Open Serial Monitor
   - Verify messages appear

3. **HC-SR04 Test**
   - Upload distance sensor test sketch
   - Move hand in front of sensor
   - Verify distance readings in Serial Monitor

4. **OLED Test**
   - Upload OLED test sketch (Adafruit_SSD1306 examples)
   - Verify display shows text/graphics

5. **RGB LED Test**
   - Set each color to HIGH sequentially
   - Verify Red, Green, Blue light up correctly

6. **Button Test**
   - Upload button test sketch
   - Press each button
   - Verify Serial Monitor shows button presses

7. **Buzzer Test**
   - Play different tones
   - Verify audible sound

8. **ESP32-CAM Test**
   - Upload CameraWebServer example
   - Verify WiFi AP appears
   - Check camera stream in browser

---

## Troubleshooting Wiring Issues

| Problem | Possible Cause | Solution |
|---------|---------------|----------|
| OLED blank | Wrong I2C pins | Use D3=SCL, D4=SDA |
| Distance always 0 | Swapped TRIG/ECHO | Check D2=TRIG, D13=ECHO |
| RGB LED dim | No resistors | Add 220Ω resistors |
| Button not responding | No pull-up | Use `INPUT_PULLUP` |
| Serial Monitor silent | Using TX/RX pins | Keep D0/D1 free |
| ESP32-CAM brownout | Weak power | Use 5V 2A supply |
| WiFi unstable | Poor power | Add capacitor, use quality PSU |
| Buzzer silent | Wrong pin mode | Use `OUTPUT` mode for A0 |

---

## Final Assembly Checklist

Before powering on:

- [ ] All GND connections to common ground
- [ ] 5V connections to power rail
- [ ] HC-SR04: VCC=5V, GND=GND, TRIG=D2, ECHO=D13
- [ ] OLED: VCC=5V, GND=GND, SDA=D4, SCL=D3
- [ ] Buttons: D5, D6, D7 to GND (with internal pull-ups)
- [ ] RGB LED: Red=D8, Green=D9, Blue=D10 (with 220Ω resistors), Cathode=GND
- [ ] Buzzer: Positive=A0, Negative=GND
- [ ] TX (D1) and RX (D0) are free
- [ ] Power supply rated 5V 2A minimum
- [ ] ESP32-CAM powered separately (5V 2A)
- [ ] No short circuits (check with multimeter)
- [ ] All components firmly connected

---

## Next Steps

After completing wiring:
1. Upload firmware to WEMOS D1 R1
2. Upload firmware to ESP32-CAM
3. Power on both boards
4. Connect to "PostureCoach-AP" WiFi
5. Open browser to http://192.168.4.1
6. Test all functionality

**Refer to:**
- [Assembly Guide](07-ASSEMBLY-GUIDE.md) for detailed assembly steps
- [User Manual](08-USER-MANUAL.md) for operation instructions
- [Troubleshooting](09-TROUBLESHOOTING.md) for common issues

### Buzzer Connection
- Connect VCC to 3.3V.
- Connect GND to GND.
- Connect the signal pin to D4 (GPIO2).

### Button Connections with Internal Pull-Ups
- Connect one terminal to D0 (GPIO0).
- Connect the other terminal to GND.
- Enable internal pull-up resistor in software.

## Power Distribution Diagram
```plaintext
+-------+
| 3.3V  |
+-------+
   |   
+---+---+      +---------+
| GND   | -->  | GND     |
|       |       |         |
|   ... |       |  ...    |
+-------+       +---------+
```

## Breadboard Layout Suggestions
- Use a standard breadboard layout with power rails on top and bottom.
- Group components based on functionality (e.g. sensors, displays).

## Wire Color Coding Recommendations
- Red for power (VCC).
- Black for ground (GND).
- Yellow for signal wires.
- Other colors for specific connections (e.g. green for I2C).

## Common Wiring Mistakes to Avoid
- Mixing up VCC and GND connections.
- Not using resistors for LEDs.
- Forgetting to enable pull-ups for buttons.

## Testing Procedures for Each Connection
- Verify each connection visually before powering devices.
- Use a multimeter to check for continuity in important connections.
- Test each component individually before integrating into the system.
