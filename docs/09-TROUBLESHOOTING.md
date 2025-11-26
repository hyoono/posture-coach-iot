# Troubleshooting Guide

## 🔧 Smart Posture Coach - Problem Resolution

This guide helps you diagnose and fix common issues with your Smart Posture & Ergonomics Camera Coach system.

---

## Quick Diagnosis

**Start here to identify your issue:**

| Symptom | Quick Check | Section |
|---------|-------------|---------|
| Dashboard won't load | WiFi connected? | [WiFi Issues](#wifi-connectivity-issues) |
| Camera feed black | Privacy mode on? | [Camera Issues](#camera-issues) |
| No distance reading | Sensor blocked? | [Sensor Issues](#sensor-issues) |
| Buttons not responding | Check wiring | [Button Issues](#button-issues) |
| LED not working | Check connections | [LED Issues](#led-issues) |
| No buzzer sound | Check A0 pin | [Buzzer Issues](#buzzer-issues) |
| OLED blank | Check I2C pins | [OLED Issues](#oled-display-issues) |
| System unstable | Power supply? | [Power Issues](#power-supply-issues) |

---

## WiFi Connectivity Issues

### Issue: PostureCoach-AP network not appearing

**Symptoms:**
- WiFi list doesn't show "PostureCoach-AP"
- Can't find network to connect to

**Diagnosis:**
```
1. Check ESP32-CAM LED - is it lit?
2. Look at Serial Monitor (115200 baud)
3. Wait 10-15 seconds after power-on
```

**Solutions:**

✅ **Solution 1: Power cycle ESP32-CAM**
```
1. Unplug ESP32-CAM power
2. Wait 5 seconds
3. Plug back in
4. Wait 10 seconds
5. Refresh WiFi list on device
```

✅ **Solution 2: Check power supply**
- ESP32-CAM needs stable 5V 2A
- Try different power adapter
- Don't power from computer USB (insufficient current)
- Add 470µF capacitor between 5V-GND

✅ **Solution 3: Re-upload firmware**
```
1. Connect ESP32-CAM via FTDI/USB
2. GPIO0 to GND (programming mode)
3. Upload firmware
4. Disconnect GPIO0
5. Press RESET button
6. Check Serial Monitor for "WiFi AP started"
```

✅ **Solution 4: Check firmware configuration**
```cpp
// Verify in esp32-cam.ino:
const char* AP_SSID = "PostureCoach-AP";
const char* AP_PASSWORD = "posture123";

// Check Serial Monitor output:
// "WiFi AP started successfully!"
// "AP SSID: PostureCoach-AP"
// "AP IP address: 192.168.4.1"
```

---

### Issue: Connected to PostureCoach-AP but no internet

**Symptom:**
- Device says "Connected"
- No internet access warning
- Apps can't access internet

**Explanation:**
✅ **This is NORMAL and EXPECTED!**

PostureCoach-AP is a **standalone local network** - it doesn't provide internet access.

**Solution:**
- No action needed - system works correctly
- Dashboard available at http://192.168.4.1
- To use internet, switch to your regular WiFi
- System designed for local operation only

---

### Issue: Can't connect to PostureCoach-AP

**Symptoms:**
- Network visible but connection fails
- "Unable to join network" error
- "Authentication failed" message

**Solutions:**

✅ **Solution 1: Verify password**
- Default password: **posture123**
- Check for typos (lowercase, no spaces)
- Password is case-sensitive

✅ **Solution 2: Forget and reconnect**
```
1. Go to WiFi settings
2. Find "PostureCoach-AP"
3. Tap "Forget Network" (iOS) or "Forget" (Android)
4. Wait 5 seconds
5. Reconnect with correct password
```

✅ **Solution 3: Check client limit**
- ESP32-CAM supports max 4 simultaneous connections
- Disconnect other devices if at limit
- Check Serial Monitor for connection count

✅ **Solution 4: Restart device**
- Restart your phone/tablet/laptop
- Clear WiFi cache
- Try connecting again

---

### Issue: WEMOS D1 R1 won't connect to ESP32-CAM

**Symptoms:**
- OLED shows "Connecting..." forever
- RGB LED blinking yellow
- Serial Monitor: WiFi connection failed

**Diagnosis:**
```cpp
// Check Serial Monitor output:
Serial.println("Connecting to: PostureCoach-AP");
// Should see dots (.) then "WiFi connected"
```

**Solutions:**

✅ **Solution 1: Verify SSID and password match**
```cpp
// In wemos-d1-r1.ino:
const char* ssid = "PostureCoach-AP";  // Must match ESP32-CAM
const char* password = "posture123";    // Must match ESP32-CAM

// Check esp32-cam.ino has same values
```

✅ **Solution 2: Power up sequence**
```
1. Power ESP32-CAM FIRST (wait 15 seconds)
2. Then power WEMOS D1 R1
3. WEMOS will auto-connect
4. Check OLED for "WiFi Connected"
```

✅ **Solution 3: Check signal strength**
- Place boards within 2 meters for setup
- Remove obstructions between boards
- Avoid metal enclosures (block WiFi)

✅ **Solution 4: Re-upload WEMOS firmware**
```
1. Open wemos-d1-r1.ino
2. Verify SSID/password
3. Upload via USB
4. Open Serial Monitor (115200 baud)
5. Watch connection process
```

---

## Camera Issues

### Issue: Camera feed shows black screen

**Symptoms:**
- Dashboard loads but camera feed is black
- Camera status shows "Camera Offline"
- No video stream visible

**Solutions:**

✅ **Solution 1: Check privacy mode**
```
1. Press WEMOS Button 3 (D7) to toggle privacy
2. Check dashboard - should show "Camera Active"
3. RGB LED should NOT be magenta
4. OLED should NOT show "Privacy ON"
```

✅ **Solution 2: Restart ESP32-CAM**
```
1. Unplug ESP32-CAM
2. Wait 5 seconds
3. Plug back in
4. Wait 10 seconds for boot
5. Refresh browser page
```

✅ **Solution 3: Check camera connection**
```
1. Power off ESP32-CAM
2. Inspect camera ribbon cable
3. Ensure cable fully inserted (both ends)
4. Cable should click into place
5. No visible damage to cable
6. Power on and test
```

✅ **Solution 4: Verify stream endpoint**
```
In browser, try:
- http://192.168.4.1/stream (should show continuous stream)
- If blank, check Serial Monitor for errors
- Look for: "Camera initialized successfully"
```

✅ **Solution 5: Check PSRAM**
```cpp
// In Serial Monitor during boot:
// Should see: "PSRAM found"
// If not, camera resolution limited

// In esp32-cam.ino:
if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;  // 640x480
} else {
    config.frame_size = FRAMESIZE_QVGA; // 320x240
}
```

---

### Issue: Camera feed is slow or laggy

**Symptoms:**
- Very low frame rate (< 1 FPS)
- Stuttering or freezing
- Long delays between frames

**Solutions:**

✅ **Solution 1: Reduce resolution**
```cpp
// In esp32-cam.ino, change to:
config.frame_size = FRAMESIZE_QVGA;  // 320x240 (faster)
// Instead of:
config.frame_size = FRAMESIZE_VGA;   // 640x480 (slower)
```

✅ **Solution 2: Check WiFi signal**
- Move closer to ESP32-CAM
- Reduce distance to < 5 meters
- Remove obstacles between devices

✅ **Solution 3: Limit simultaneous connections**
- Only one dashboard open at a time
- Close other browser tabs accessing stream
- Disconnect unused devices from PostureCoach-AP

✅ **Solution 4: Power supply check**
- Ensure 5V 2A minimum
- Camera draws significant current during streaming
- Use quality power adapter (not cheap USB charger)

---

### Issue: Camera image quality poor

**Symptoms:**
- Blurry or out of focus
- Dark or overexposed
- Grainy or noisy

**Solutions:**

✅ **Solution 1: Focus adjustment**
```
1. Power off ESP32-CAM
2. Gently rotate camera lens (very small turns)
3. Some OV2640 modules have fixed focus
4. Power on and test
```

✅ **Solution 2: Lighting adjustment**
- Add more light to area
- Avoid backlighting (window behind you)
- Use diffused lighting (not direct)
- Position light source in front

✅ **Solution 3: Clean lens**
```
1. Power off system
2. Use microfiber lens cloth
3. Gently clean camera lens
4. Don't use liquids
5. Power on and test
```

✅ **Solution 4: Adjust camera settings**
```cpp
// In esp32-cam.ino, tune these values:
sensor_t * s = esp_camera_sensor_get();
s->set_brightness(s, 0);     // -2 to 2 (try 1 for darker rooms)
s->set_contrast(s, 0);       // -2 to 2
s->set_saturation(s, 0);     // -2 to 2
s->set_special_effect(s, 0); // 0 = No Effect
```

---

## Sensor Issues

### Issue: Distance always shows "--" or 0

**Symptoms:**
- No distance reading on dashboard
- OLED shows "Dist: -- cm"
- No distance alerts

**Solutions:**

✅ **Solution 1: Check sensor wiring**
```
Verify connections:
HC-SR04 VCC  → WEMOS 5V
HC-SR04 GND  → WEMOS GND
HC-SR04 TRIG → WEMOS D2 (GPIO4)
HC-SR04 ECHO → WEMOS D13 (GPIO14)

Common mistakes:
- Swapped TRIG and ECHO
- Using 3.3V instead of 5V
- Loose connections
```

✅ **Solution 2: Check for obstructions**
- Clear path in front of sensor
- No objects within 2cm
- Sensor facing correct direction
- Remove any covering/tape

✅ **Solution 3: Test sensor**
```
Upload simple HC-SR04 test sketch:
1. Read distance every second
2. Print to Serial Monitor
3. Move hand in front of sensor
4. Verify changing readings
```

✅ **Solution 4: Replace sensor**
- HC-SR04 sensors can fail
- Test with another HC-SR04
- Check for visible damage
- Verify voltage on VCC pin (should be 5V)

---

### Issue: Distance readings inaccurate

**Symptoms:**
- Distance shows wrong value
- Inconsistent measurements
- Jumping between values

**Solutions:**

✅ **Solution 1: Calibrate positioning**
- Mount sensor firmly (no vibration)
- Aim perpendicular to target surface
- Avoid angled surfaces (causes reflection errors)
- Target should be flat for best accuracy

✅ **Solution 2: Check interference**
- Move away from other ultrasonic sensors
- Avoid extremely loud sounds
- Keep away from soft/absorbent materials
- Hard surfaces reflect better

✅ **Solution 3: Software filtering**
```cpp
// Add moving average filter:
float readDistanceFiltered() {
  const int samples = 5;
  float sum = 0;
  for(int i = 0; i < samples; i++) {
    float d = readDistance();
    if(d > 0) sum += d;
    delay(60);  // Wait between readings
  }
  return sum / samples;
}
```

✅ **Solution 4: Check measurement range**
- HC-SR04 accurate from 2cm to 400cm
- Optimal range: 40cm to 80cm
- Outside range: unreliable readings
- For posture: 50-70cm ideal

---

## Button Issues

### Issue: Buttons don't respond

**Symptoms:**
- Pressing button has no effect
- No Serial Monitor output on press
- No action taken

**Solutions:**

✅ **Solution 1: Check wiring**
```
Verify button connections:
Button 1: One side to D5, other to GND
Button 2: One side to D6, other to GND
Button 3: One side to D7, other to GND

NO external pull-up resistors needed!
Internal pull-ups enabled in code.
```

✅ **Solution 2: Verify internal pull-ups**
```cpp
// In wemos-d1-r1.ino setup():
pinMode(BUTTON1_PIN, INPUT_PULLUP);  // D5
pinMode(BUTTON2_PIN, INPUT_PULLUP);  // D6
pinMode(BUTTON3_PIN, INPUT_PULLUP);  // D7

// Button logic: pressed = LOW, released = HIGH
```

✅ **Solution 3: Test button hardware**
```
1. Power off WEMOS
2. Use multimeter (continuity mode)
3. Test button: should beep when pressed
4. Check for broken wires
5. Verify solder joints
```

✅ **Solution 4: Check debounce timing**
```cpp
// Increase debounce delay if needed:
#define BUTTON_DEBOUNCE 100  // Increase to 100ms

// In button check:
if (millis() - lastButton1Press > BUTTON_DEBOUNCE) {
  // Button action
}
```

---

### Issue: Buttons trigger multiple times

**Symptoms:**
- Single press registers multiple times
- Actions repeat unexpectedly
- Button "bouncing"

**Solutions:**

✅ **Solution 1: Increase debounce delay**
```cpp
// In wemos-d1-r1.ino:
#define BUTTON_DEBOUNCE 200  // Increase from 50 to 200ms
```

✅ **Solution 2: Add hardware capacitor**
```
Add 0.1µF capacitor across button:
- One leg to button pin (D5/D6/D7)
- Other leg to GND
- Reduces electrical noise
```

✅ **Solution 3: Check button quality**
- Cheap tactile switches can bounce more
- Use quality momentary pushbuttons
- Replace suspect buttons

---

## LED Issues

### Issue: RGB LED not lighting up

**Symptoms:**
- LED stays off
- No color changes
- All pins dark

**Solutions:**

✅ **Solution 1: Check LED type**
```
Verify you have COMMON CATHODE RGB LED:
- Longest pin = Cathode (connect to GND)
- Other 3 pins = Red, Green, Blue anodes

NOT Common Anode (would need different wiring)
```

✅ **Solution 2: Verify wiring**
```
Common Cathode RGB LED connections:
Red Anode   → [220Ω resistor] → D8
Green Anode → [220Ω resistor] → D9
Blue Anode  → [220Ω resistor] → D10
Cathode     → (direct) → GND

Check:
- Resistors present (220Ω each)
- Correct pins (D8, D9, D10)
- Cathode to GND (longest pin)
```

✅ **Solution 3: Test LED directly**
```
1. Power off WEMOS
2. Connect red anode → 220Ω → 3.3V
3. Cathode → GND
4. LED should light red
5. Repeat for green and blue
6. If LED doesn't light, it's damaged
```

✅ **Solution 4: Check pin modes**
```cpp
// In wemos-d1-r1.ino setup():
pinMode(LED_RED, OUTPUT);    // D8
pinMode(LED_GREEN, OUTPUT);  // D9
pinMode(LED_BLUE, OUTPUT);   // D10

// Test with direct write:
digitalWrite(LED_RED, HIGH);  // Should light red
```

---

### Issue: LED shows wrong colors

**Symptoms:**
- Red appears green
- Colors mixed up
- Unexpected color combinations

**Solutions:**

✅ **Solution 1: Verify pin assignments**
```cpp
// Check pin definitions match wiring:
#define LED_RED D8     // Should go to red anode
#define LED_GREEN D9   // Should go to green anode
#define LED_BLUE D10   // Should go to blue anode

// Test each color separately:
analogWrite(LED_RED, 255);
analogWrite(LED_GREEN, 0);
analogWrite(LED_BLUE, 0);
// Should be pure red
```

✅ **Solution 2: Check for crossed wires**
- Trace each wire from WEMOS to LED
- Verify red wire to red anode, etc.
- Use multimeter to confirm connections

✅ **Solution 3: Update color mappings**
```cpp
// If wired differently, adjust code:
void setRGBColor(int r, int g, int b) {
  analogWrite(LED_RED, r);    // If red is actually on D9
  analogWrite(LED_GREEN, g);  // If green is actually on D8
  analogWrite(LED_BLUE, b);   // Adjust as needed
}
```

---

## Buzzer Issues

### Issue: No sound from buzzer

**Symptoms:**
- Buzzer completely silent
- No audio feedback
- Alert patterns don't play

**Solutions:**

✅ **Solution 1: Check buzzer wiring**
```
Verify connections:
Buzzer (+) → A0 (WEMOS pin)
Buzzer (-) → GND

Common issues:
- Reversed polarity (swap wires)
- Wrong pin (ensure using A0)
- Loose connection
```

✅ **Solution 2: Test buzzer directly**
```
1. Power off WEMOS
2. Connect buzzer (+) to 3.3V briefly
3. Should hear click/pop
4. If silent, buzzer may be damaged
5. Try different buzzer
```

✅ **Solution 3: Check pin mode and test**
```cpp
// In setup():
pinMode(BUZZER_PIN, OUTPUT);  // A0

// Simple test in setup():
tone(BUZZER_PIN, 1000, 1000);  // 1000Hz for 1 second
delay(1100);
noTone(BUZZER_PIN);
// Should hear 1-second beep on startup
```

✅ **Solution 4: Volume issue**
- Some buzzers are quiet
- Move buzzer closer to hear
- Use active buzzer (louder)
- Check for physical obstructions

---

### Issue: Buzzer too loud

**Symptoms:**
- Buzzer annoyingly loud
- Can't reduce volume
- Disturbs others

**Solutions:**

✅ **Solution 1: Use snooze button**
- Press Button 2 (D6) to silence alerts for 5 minutes
- Temporary solution during meetings, calls, etc.

✅ **Solution 2: Reduce alert sensitivity**
```
1. Open dashboard settings
2. Lower "Alert Sensitivity" slider
3. Fewer alerts = less buzzer noise
```

✅ **Solution 3: Physical dampening**
- Cover buzzer with tape (reduces volume)
- Use foam/cotton over buzzer
- Mount buzzer upside-down in enclosure

✅ **Solution 4: Code modification**
```cpp
// Reduce buzzer duration:
void playBuzzerTone(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration / 2);  // Half duration
}

// Or comment out urgent patterns:
void executeBuzzerCommand(String pattern, int duration) {
  // if (pattern == "URGENT") { return; }  // Skip urgent alerts
  // Keep other patterns...
}
```

---

## OLED Display Issues

### Issue: OLED screen blank

**Symptoms:**
- Display completely blank/black
- No text visible
- Backlight may or may not be on

**Solutions:**

✅ **Solution 1: Check I2C connections**
```
Critical: Custom I2C pins used!
OLED VCC → WEMOS 5V
OLED GND → WEMOS GND
OLED SDA → WEMOS D4 (NOT D2!)
OLED SCL → WEMOS D3 (NOT D1!)

Note: Default I2C is D1/D2, but we use D3/D4
to keep TX/RX free for Serial Monitor
```

✅ **Solution 2: Verify I2C initialization**
```cpp
// In wemos-d1-r1.ino setup():
#define OLED_SDA D4  // GPIO2
#define OLED_SCL D3  // GPIO0

Wire.begin(OLED_SDA, OLED_SCL);  // CRITICAL: Custom pins!
// NOT: Wire.begin(); // This uses default D1/D2

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
  Serial.println("OLED init failed");
  // Check this message in Serial Monitor
}
```

✅ **Solution 3: Check I2C address**
```cpp
// Most OLED displays use 0x3C, some use 0x3D
// Try both:
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Try first
// If fails, try:
display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // Alternative
```

✅ **Solution 4: I2C scanner**
```cpp
// Upload I2C scanner sketch to find address:
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(D4, D3);  // Custom pins!
  
  Serial.println("Scanning I2C...");
  for(byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if(Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
    }
  }
}
```

---

### Issue: OLED shows garbled text

**Symptoms:**
- Random characters
- Partial text
- Corrupted display

**Solutions:**

✅ **Solution 1: Check power supply**
- OLED needs stable power
- Try 5V instead of 3.3V
- Add 100µF capacitor across VCC-GND

✅ **Solution 2: Reduce I2C speed**
```cpp
// In wemos-d1-r1.ino:
Wire.begin(OLED_SDA, OLED_SCL);
Wire.setClock(100000);  // 100kHz (default is 400kHz)
// Slower speed = more reliable
```

✅ **Solution 3: Shorten I2C wires**
- Keep wires under 10cm
- Use quality jumper wires
- Avoid coiled/tangled wires
- Twisted pair wiring helps

---

## Dashboard Issues

### Issue: Dashboard won't load

**Symptoms:**
- Browser shows "Can't reach this page"
- "Connection timed out"
- "Server not found"

**Solutions:**

✅ **Solution 1: Verify WiFi connection**
```
1. Check device connected to "PostureCoach-AP"
2. WiFi icon should show connected
3. Open WiFi settings to confirm
4. Disconnect and reconnect if uncertain
```

✅ **Solution 2: Try correct URL**
```
Try these URLs in order:
1. http://192.168.4.1
2. http://192.168.4.1/index.html
3. http://192.168.4.1/

Don't use:
- https:// (no SSL/TLS)
- www.192.168.4.1
- Different IP addresses
```

✅ **Solution 3: Clear browser cache**
```
1. Press Ctrl+Shift+Delete (Ctrl+Shift+⌫ on Mac)
2. Select "Cached images and files"
3. Clear cache
4. Close and reopen browser
5. Try loading dashboard again
```

✅ **Solution 4: Try different browser**
- Chrome/Edge (recommended)
- Firefox
- Safari
- Avoid very old browsers (IE)

✅ **Solution 5: Check ESP32-CAM Serial Monitor**
```
Look for:
"HTTP server started on port 80"
"Dashboard: http://192.168.4.1/"

If not present:
- SPIFFS mount may have failed
- Re-upload SPIFFS data
- Re-upload firmware
```

---

### Issue: Dashboard loads but updates stop

**Symptoms:**
- Dashboard loads initially
- Data stops updating
- "Connection Lost" status

**Solutions:**

✅ **Solution 1: Check WiFi stability**
```
Dashboard polls every 3 seconds
If connection drops:
1. Move closer to ESP32-CAM
2. Check WiFi signal strength
3. Reduce distance/obstacles
4. Refresh browser page
```

✅ **Solution 2: Reduce simultaneous connections**
- Close duplicate browser tabs
- Disconnect unused devices
- ESP32-CAM supports max 4 connections

✅ **Solution 3: Refresh page**
- Press F5 or Ctrl+R (Ctrl+⌘R on Mac)
- Hard refresh: Ctrl+Shift+R (Ctrl+Shift+⌘R on Mac)

---

## Power Supply Issues

### Issue: System randomly resets or crashes

**Symptoms:**
- ESP32-CAM reboots unexpectedly
- WEMOS D1 R1 restarts
- Erratic behavior
- "Brownout detector" in Serial Monitor

**Solutions:**

✅ **Solution 1: Upgrade power supply**
```
Requirements:
- ESP32-CAM: 5V 2A minimum (camera streaming is power-hungry)
- WEMOS D1 R1: 5V 1A minimum
- Use quality wall adapters (not cheap USB chargers)
- Avoid powering from computer USB (insufficient current)
```

✅ **Solution 2: Add bulk capacitor**
```
ESP32-CAM especially:
- 470µF to 1000µF electrolytic capacitor
- Positive leg to 5V
- Negative leg to GND
- Place close to ESP32-CAM module
- Smooths voltage spikes during camera operation
```

✅ **Solution 3: Separate power supplies**
```
Use 2 dedicated supplies:
1. One for ESP32-CAM (5V 2A)
2. One for WEMOS D1 R1 (5V 1A)
3. Connect GND together (common ground)
4. Separate VCC (reduces interference)
```

✅ **Solution 4: Check wiring and connections**
- Loose power connections cause voltage drops
- Ensure firm connections
- Check for corroded contacts
- Use quality breadboard or PCB

---

### Issue: Board gets hot

**Symptoms:**
- ESP32-CAM warm/hot to touch
- WEMOS D1 R1 heating up
- Burning smell (STOP IMMEDIATELY if this occurs)

**Solutions:**

✅ **Solution 1: Normal operating temperature**
- ESP32-CAM can get warm during streaming (40-50°C)
- This is usually normal
- Ensure adequate airflow

✅ **Solution 2: Check for shorts**
```
STOP and check immediately:
1. Power off system
2. Visually inspect for:
   - Crossed wires
   - Exposed copper touching
   - Solder bridges
3. Use multimeter to check for shorts between VCC-GND
4. Fix any shorts before powering on
```

✅ **Solution 3: Add cooling**
- Add small heatsink to ESP32-CAM module
- Ensure airflow around boards
- Don't enclose tightly
- Use small USB fan if needed

---

## Serial Monitor Issues

### Issue: No output in Serial Monitor

**Symptoms:**
- Serial Monitor blank
- No boot messages
- No debug output

**Solutions:**

✅ **Solution 1: Check baud rate**
```
Both firmwares use: 115200 baud

In Arduino IDE Serial Monitor:
1. Bottom-right dropdown
2. Select "115200 baud"
3. Close and reopen Serial Monitor
```

✅ **Solution 2: Verify USB connection**
- Use quality USB cable (data, not power-only)
- Try different USB port
- Check cable for damage

✅ **Solution 3: Select correct port**
```
In Arduino IDE:
Tools → Port → Select correct COM port

On Windows: COM3, COM4, etc.
On Mac: /dev/cu.usbserial...
On Linux: /dev/ttyUSB0, etc.
```

✅ **Solution 4: Install drivers**
- ESP32-CAM may need CP2102 or CH340 driver
- WEMOS D1 R1 may need CH340 driver
- Download from manufacturer website

---

## General Troubleshooting Steps

### When nothing works:

1. **Start from scratch:**
   ```
   1. Power off everything
   2. Disconnect all wires
   3. Check each component individually
   4. Reconnect one at a time
   5. Test after each connection
   ```

2. **Verify firmware:**
   ```
   1. Re-download latest code from GitHub
   2. Check configuration values
   3. Re-upload to both boards
   4. Monitor Serial output during boot
   ```

3. **Test components separately:**
   ```
   1. Upload simple test sketches
   2. Test HC-SR04 alone
   3. Test OLED alone
   4. Test buttons alone
   5. Isolate the failing component
   ```

4. **Check documentation:**
   - [Wiring Diagrams](04-WIRING-DIAGRAMS.md)
   - [API Specification](06-API-SPECIFICATION.md)
   - [User Manual](08-USER-MANUAL.md)

---

## Getting Help

### Before asking for help:

✅ Provide this information:
- Exact symptoms
- Steps to reproduce
- Serial Monitor output
- Photos of wiring (clear, well-lit)
- Arduino IDE version
- Board versions (ESP32 core, ESP8266 core)
- What you've already tried

### Where to get help:

- **GitHub Issues:** https://github.com/hyoono/posture-coach-iot/issues
- **Project Wiki:** https://github.com/hyoono/posture-coach-iot/wiki
- **ESP32 Forum:** https://esp32.com/
- **Arduino Forum:** https://forum.arduino.cc/

---

## Diagnostic Checklist

Print this and check off as you verify:

**ESP32-CAM:**
- [ ] Powers on (LED lit)
- [ ] Creates WiFi AP ("PostureCoach-AP" visible)
- [ ] Serial Monitor shows "System Ready"
- [ ] Dashboard accessible at 192.168.4.1
- [ ] Camera stream works at /stream
- [ ] API endpoints respond

**WEMOS D1 R1:**
- [ ] Powers on (LED lit)
- [ ] Connects to PostureCoach-AP WiFi
- [ ] OLED displays info
- [ ] RGB LED lights up
- [ ] Buzzer makes sound
- [ ] All 3 buttons respond
- [ ] Distance sensor reads values

**Communication:**
- [ ] WEMOS sends distance data to ESP32-CAM
- [ ] ESP32-CAM sends commands to WEMOS
- [ ] Dashboard polls successfully
- [ ] LED changes with posture score
- [ ] Buzzer alerts work

---

**Still stuck?** Document your issue thoroughly and post on GitHub Issues. Include:
- Photos of setup
- Serial Monitor logs
- Steps attempted
- Expected vs actual behavior

---

**Version:** 1.0.0  
**Last Updated:** December 2024
