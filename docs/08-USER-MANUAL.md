# User Manual

## 👤 Smart Posture Coach - User Guide

Welcome to the Smart Posture & Ergonomics Camera Coach! This manual will help you set up, use, and get the most out of your posture monitoring system.

---

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [First-Time Setup](#first-time-setup)
3. [Daily Usage](#daily-usage)
4. [Understanding the Dashboard](#understanding-the-dashboard)
5. [Using Physical Controls](#using-physical-controls)
6. [Posture Scoring System](#posture-scoring-system)
7. [Break Management](#break-management)
8. [Settings Configuration](#settings-configuration)
9. [Maintenance](#maintenance)
10. [FAQ](#faq)

---

## Quick Start Guide

### What You Need
- Assembled Smart Posture Coach system (ESP32-CAM + WEMOS D1 R1)
- 2× 5V power supplies (or 2 USB cables)
- WiFi-enabled device (smartphone, tablet, or laptop)
- Web browser (Chrome, Firefox, Safari, or Edge)

### 5-Minute Setup

1. **Power On**
   - Plug in ESP32-CAM (wait 10 seconds)
   - Plug in WEMOS D1 R1 (wait 5 seconds)

2. **Connect to WiFi**
   - On your device, open WiFi settings
   - Look for network: **PostureCoach-AP**
   - Connect using password: **posture123**

3. **Open Dashboard**
   - Open web browser
   - Navigate to: **http://192.168.4.1**
   - Dashboard should load with live camera feed

4. **Position Camera**
   - Adjust ESP32-CAM to capture your face and shoulders
   - Sit with good posture
   - You're ready to start!

---

## First-Time Setup

### Step 1: Power Up the System

1. Connect ESP32-CAM to 5V 2A power supply
   - Red LED should light up
   - Wait 10 seconds for bootup
   - Check Serial Monitor if available (115200 baud)

2. Connect WEMOS D1 R1 to 5V power supply
   - Built-in LED should light up
   - OLED display shows "Connecting to PostureCoach-AP"
   - RGB LED turns green when connected

**Expected Behavior:**
- ESP32-CAM creates WiFi network "PostureCoach-AP"
- WEMOS D1 R1 automatically connects to ESP32-CAM
- OLED shows "WiFi Connected" and IP address

### Step 2: Connect Your Device

#### On Smartphone (iOS/Android):
1. Open **Settings** → **WiFi**
2. Tap **PostureCoach-AP**
3. Enter password: **posture123**
4. Wait for "Connected" message
5. Note: Internet won't work (this is normal - it's a local network)

#### On Computer (Windows/Mac/Linux):
1. Click WiFi icon in system tray
2. Select **PostureCoach-AP**
3. Enter password: **posture123**
4. Click Connect

⚠️ **Important:** Your device won't have internet while connected to PostureCoach-AP. This is a standalone local network.

### Step 3: Access the Dashboard

1. Open your preferred web browser
2. Type in address bar: **http://192.168.4.1**
3. Press Enter
4. Dashboard should load in 2-3 seconds

**If dashboard doesn't load:**
- Check WiFi connection (should show "PostureCoach-AP")
- Verify ESP32-CAM is powered on
- Try: **http://192.168.4.1/index.html**
- See [Troubleshooting](#troubleshooting) section

### Step 4: Camera Positioning

1. Place ESP32-CAM at eye level or slightly above
2. Position 1-2 feet away from your normal sitting position
3. Ensure camera can see:
   - Your entire face
   - Both shoulders
   - Upper torso

**Good Positioning:**
```
        [ESP32-CAM]
             ↓
          ┌─────┐
          │  😊  │ ← Face clearly visible
          │ ╱│╲ │ ← Shoulders in frame
          └─────┘
```

**Poor Positioning:**
- Too close (only face visible)
- Too far (can't detect features)
- Angle too steep (distorted perspective)

### Step 5: Initial Calibration

1. Sit with **proper posture:**
   - Back straight
   - Shoulders relaxed
   - Eyes at screen level
   - Distance 50-70cm from screen

2. Check dashboard:
   - Posture score should be 80-100
   - Status should show "Good" or "Excellent"
   - Distance reading 50-70cm

3. If score is low:
   - Adjust your posture
   - Reposition camera if needed
   - Wait 3 seconds for update

You're all set! The system will now monitor your posture automatically.

---

## Daily Usage

### Starting Your Session

1. Power on both boards (if not already on)
2. Connect device to PostureCoach-AP WiFi
3. Open dashboard (http://192.168.4.1)
4. Sit with good posture to establish baseline

### Normal Operation

The system works automatically:
- **Camera** captures frames every 2 seconds
- **Posture analysis** runs continuously
- **Distance sensor** reads every 2 seconds
- **Dashboard** updates every 3 seconds
- **OLED display** shows real-time score
- **RGB LED** indicates posture quality
- **Buzzer** alerts on poor posture

### What to Expect

#### Good Posture Session:
- Score: 80-100 (green LED)
- Status: "Good" or "Excellent"
- Minimal or no buzzer alerts
- Dashboard shows consistent scores

#### Poor Posture Detected:
- Score: Below 60 (red LED)
- Status: "Poor" or "Fair"
- Buzzer sounds (2-3 beeps)
- Dashboard shows red indicators
- OLED displays warning

### Throughout the Day

**Every 45 minutes:**
- System reminds you to take a break
- Buzzer plays gentle two-tone
- Dashboard shows "Break Recommended"
- RGB LED turns blue

**During breaks:**
- Posture monitoring pauses
- 5-minute countdown on OLED
- Camera stream continues (unless privacy mode)
- Resume automatically after break

### Ending Your Session

1. Review session statistics on dashboard
2. Note average score and alerts triggered
3. Simply close browser or disconnect WiFi
4. System continues monitoring for next user

**Optional:** Power off both boards if not using for extended period.

---

## Understanding the Dashboard

### Live Camera Feed

```
┌─────────────────────────────┐
│   [Camera Active] 🟢        │
│                             │
│     📷 Live Feed            │
│     [Your face/shoulders]   │
│                             │
└─────────────────────────────┘
```

**What you see:**
- Real-time MJPEG stream (10 FPS)
- Green indicator = camera active
- Updates automatically (no refresh needed)

**Camera Status:**
- 🟢 Green dot + "Camera Active" = Working
- 🔴 Red dot + "Camera Offline" = Connection issue
- Black screen = Privacy mode enabled

### Posture Score Gauge

```
        ┌───────┐
        │  85   │  ← Your score (0-100)
        │ /100  │
        └───────┘
     Circular gauge
     (color changes)
```

**Score Colors:**
- **Green (90-100):** Excellent posture - keep it up!
- **Blue (75-89):** Good posture - minor improvements possible
- **Yellow (60-74):** Fair posture - adjust your position
- **Red (<60):** Poor posture - immediate correction needed

**Score Updates:**
- Refreshes every 3 seconds
- Smooth animation transitions
- Real-time visual feedback

### Distance Monitor

```
|----[30cm]----|----[50-70cm OPTIMAL]----|----[90cm]----|
   Too Close          Perfect!              Too Far
      ��                 🟢                    🟡
```

**Distance Indicator:**
- Shows current eye-to-screen distance
- Optimal zone: 50-70cm (green)
- Too close: <50cm (red)
- Too far: >70cm (yellow)

**Why It Matters:**
- Sitting too close strains eyes
- Sitting too far affects posture detection
- Optimal distance for health and accuracy

### Session Statistics

Four cards show:

1. **Duration** ⏱️
   - Current session time (HH:MM:SS)
   - Starts when system boots

2. **Average Score** 📊
   - Mean posture score for session
   - Higher is better (aim for 80+)

3. **Alerts Triggered** 🔔
   - Number of poor posture warnings
   - Lower is better

4. **Breaks Taken** ☕
   - Number of breaks during session
   - Recommended: 1 per hour

### Control Buttons

Four quick action buttons:

1. **☕ Take Break**
   - Manually start 5-minute break
   - Pauses monitoring
   - Use when needed (stretch, walk, etc.)

2. **⏰ Snooze (5 min)**
   - Silence alerts for 5 minutes
   - Use when unable to adjust posture
   - Monitoring continues

3. **🔒 Privacy Mode**
   - Stops camera feed
   - Pauses posture detection
   - Toggle on/off as needed

4. **⚙️ Settings**
   - Opens settings modal
   - Adjust sensitivity, intervals, etc.
   - Changes save automatically

---

## Using Physical Controls

### WEMOS D1 R1 has 3 buttons:

### Button 1 (D5): Manual Break
```
┌───────┐
│ BTN 1 │ → Press to start break immediately
└───────┘
```

**Function:** Start 5-minute break
**When to use:** Feel tired, need to stretch, eye strain
**Effect:**
- Break mode activates instantly
- OLED shows countdown timer
- RGB LED turns blue
- Dashboard shows "Break Mode"

**How to use:**
1. Press and release Button 1
2. Hear confirmation beep (2 tones)
3. Take your break
4. System resumes automatically after 5 minutes

### Button 2 (D6): Snooze Alerts
```
┌───────┐
│ BTN 2 │ → Press to snooze alerts for 5 minutes
└───────┘
```

**Function:** Silence buzzer alerts temporarily
**When to use:** In a meeting, on a call, need quiet time
**Effect:**
- Alerts silenced for 5 minutes
- Monitoring continues (score still tracked)
- OLED shows "Snoozed"
- Dashboard shows snooze status

**How to use:**
1. Press and release Button 2
2. Hear single confirmation beep
3. Alerts silenced for 5 minutes
4. Alerts resume automatically

### Button 3 (D7): Privacy Mode
```
┌───────┐
│ BTN 3 │ → Press to toggle camera on/off
└───────┘
```

**Function:** Enable/disable camera and monitoring
**When to use:** Privacy needed, someone else using desk, etc.
**Effect:**
- Camera stream stops
- Posture detection pauses
- RGB LED turns magenta
- OLED shows "Privacy ON"

**How to use:**
1. Press and release Button 3
2. Hear 3 rapid beeps (privacy ON) or 2 rising tones (privacy OFF)
3. RGB LED changes to magenta (ON) or green (OFF)
4. Press again to resume monitoring

---

## RGB LED Indicators

The RGB LED provides instant visual feedback:

### Color Meanings

| Color | Status | Score Range | Meaning |
|-------|--------|-------------|---------|
| 🟢 **Green** | Excellent | 90-100 | Perfect posture! Keep it up! |
| 🔵 **Blue** | Good / Break | 75-89 | Good posture or break active |
| 🟡 **Yellow** | Fair | 60-74 | Needs improvement |
| 🔴 **Red** | Poor | 0-59 | Immediate correction needed |
| 🟣 **Magenta** | Privacy | - | Privacy mode enabled |
| 🟡 **Yellow (blinking)** | WiFi | - | Connecting/reconnecting |

### LED Patterns

- **Solid color:** Normal operation
- **Blinking:** Connection issue or state change
- **Off:** System powered off

---

## Buzzer Alert Patterns

The buzzer provides audio feedback for different situations:

### Pattern Types

1. **Success (Startup/Confirmation)**
   - Rising tones: 800Hz → 1000Hz → 1200Hz
   - Duration: ~400ms
   - Means: System ready or action confirmed

2. **Warning (Fair Posture)**
   - Two medium beeps at 1000Hz
   - Duration: 200ms each, 250ms apart
   - Means: Posture needs improvement

3. **Urgent (Poor Posture)**
   - Three loud ascending beeps: 1200Hz → 1400Hz → 1600Hz
   - Duration: 150ms each
   - Means: Immediate posture correction needed

4. **Distance Alert (Too Close/Far)**
   - Rapid short beeps at 1800Hz (5 times)
   - Duration: 80ms each, 100ms apart
   - Means: Adjust distance from screen

5. **Break Reminder**
   - Gentle two-tone: 800Hz → 600Hz
   - Duration: 250ms each, 300ms apart
   - Means: Time for a break

### Volume Control

⚠️ Buzzer volume is fixed in hardware. To reduce volume:
- Use "Snooze" button during quiet times
- Enable privacy mode when alerts not needed
- Adjust sensitivity in settings to reduce alert frequency

---

## Posture Scoring System

### How Scores Are Calculated

The system analyzes multiple factors:

1. **Head Tilt (0-30 points penalty)**
   - Forward head lean detected
   - Penalty increases with tilt angle
   - Optimal: < 15° forward

2. **Shoulder Alignment (0-25 points penalty)**
   - Slouching or uneven shoulders
   - Binary: good or poor
   - Optimal: Level shoulders, upright

3. **Distance (0-20 points penalty)**
   - Too close: < 50cm
   - Too far: > 70cm
   - Optimal: 50-70cm range

4. **Sitting Time (0-15 points penalty)**
   - Prolonged sitting without breaks
   - >45 min = penalty
   - Optimal: Break every 45 min

**Formula:**
```
Score = 100 - (head_penalty + shoulder_penalty + distance_penalty + time_penalty)
```

### Score Interpretation

| Score | Status | Posture Quality | Action Needed |
|-------|--------|----------------|---------------|
| 90-100 | Excellent | Perfect posture | Maintain current position |
| 75-89 | Good | Minor issues | Small adjustments helpful |
| 60-74 | Fair | Needs improvement | Adjust posture soon |
| <60 | Poor | Significant issues | Immediate correction required |

### Improving Your Score

**If score is 60-74 (Yellow):**
- Check head position (not leaning forward)
- Straighten shoulders
- Adjust distance to 50-70cm
- Take break if sitting >45 minutes

**If score is <60 (Red):**
- Sit up straight immediately
- Move head back (chin tuck)
- Roll shoulders back
- Adjust chair height
- Take a short break

### Tips for Maintaining Good Posture

✅ **DO:**
- Keep monitor at eye level
- Sit back in chair with lumbar support
- Feet flat on floor
- Shoulders relaxed
- Take regular breaks

❌ **DON'T:**
- Lean forward toward screen
- Slouch or hunch
- Cross legs for extended periods
- Ignore alerts
- Skip breaks

---

## Break Management

### Automatic Break Reminders

**System reminds you every 45 minutes:**
1. Buzzer plays gentle two-tone
2. Dashboard shows "Break Recommended"
3. OLED displays "Time for Break!"
4. RGB LED turns blue

**What to do:**
- Press "Take Break" button (dashboard or WEMOS Button 1)
- Break timer starts (5 minutes)
- Walk around, stretch, rest eyes
- System resumes automatically

### Manual Breaks

**Start a break anytime:**
- Press WEMOS Button 1 (D5)
- OR click "Take Break" on dashboard
- Break confirmed with 2 beeps

**During break:**
- Monitoring paused for 5 minutes
- Timer countdown shown on OLED
- Camera stream continues (unless privacy mode)
- Dashboard shows break status

**Ending break:**
- Automatic after 5 minutes
- Hear confirmation beep
- Monitoring resumes
- LED returns to posture color

### Break Best Practices

**Every 45 minutes:**
- Stand up and walk
- Stretch arms and shoulders
- Look away from screen (20-20-20 rule)
- Drink water

**20-20-20 Rule:**
- Every 20 minutes
- Look at something 20 feet away
- For 20 seconds
- Reduces eye strain

### Customizing Break Intervals

In Settings modal:
1. Click "Settings" button
2. Find "Break Interval" dropdown
3. Choose: 30, 45, or 60 minutes
4. Click "Save Settings"

**Recommendations:**
- 30 min: Intensive computer work
- 45 min: Normal office work (default)
- 60 min: Light computer use

---

## Settings Configuration

### Accessing Settings

1. Open dashboard (http://192.168.4.1)
2. Click **⚙️ Settings** button
3. Modal opens with configuration options

### Available Settings

#### 1. Alert Sensitivity (0-100%)

```
Alert Sensitivity: [=========•] 75%
                    0         100
```

**What it does:** Controls how sensitive the system is to poor posture

**Values:**
- **Low (0-50%):** Only alerts on severe posture issues
- **Medium (50-75%):** Balanced (recommended)
- **High (75-100%):** Alerts on minor posture deviations

**When to adjust:**
- Getting too many alerts → Decrease sensitivity
- Not enough reminders → Increase sensitivity

#### 2. Break Interval (30/45/60 minutes)

```
Break Interval: [45 minutes ▼]
```

**What it does:** Sets time between break reminders

**Options:**
- 30 minutes: Intensive work, maximum breaks
- 45 minutes: Standard office work (default)
- 60 minutes: Light computer use, fewer breaks

**Choose based on:**
- Work intensity
- Personal preference
- Health recommendations

#### 3. Minimum Distance (30-70 cm)

```
Minimum Distance (cm): [50] ← Adjust with arrows
```

**What it does:** Sets lower bound of optimal distance range

**Range:** 30-70 cm
**Default:** 50 cm
**Recommended:** 50 cm (arm's length)

**Adjust if:**
- Alert warns "too close" when comfortable
- Desk setup requires different distance

#### 4. Maximum Distance (50-100 cm)

```
Maximum Distance (cm): [70] ← Adjust with arrows
```

**What it does:** Sets upper bound of optimal distance range

**Range:** 50-100 cm
**Default:** 70 cm

**Adjust if:**
- Alert warns "too far" when comfortable
- Large monitor requires greater distance

### Saving Settings

1. Adjust desired values
2. Click **"Save Settings"** button
3. Confirmation message appears
4. Settings take effect immediately
5. Modal closes automatically

### Resetting to Defaults

To reset settings:
1. Open Settings modal
2. Set values manually to defaults:
   - Alert Sensitivity: 75%
   - Break Interval: 45 minutes
   - Min Distance: 50 cm
   - Max Distance: 70 cm
3. Click "Save Settings"

---

## Maintenance

### Daily Care

**WEMOS D1 R1:**
- Wipe OLED screen with dry microfiber cloth
- Check button functionality
- Verify LED is visible

**ESP32-CAM:**
- Clean camera lens with lens cloth (weekly)
- Check camera angle and position
- Ensure adequate airflow (don't cover)

### Power Management

**Continuous Operation:**
- System designed for 24/7 operation
- Minimal power consumption (~2W total)
- No need to power off daily

**Extended Non-Use:**
- Power off both boards if not using >1 week
- Disconnect from power supply
- Store in dry location

### Troubleshooting Common Issues

See detailed [Troubleshooting Guide](09-TROUBLESHOOTING.md)

**Quick fixes:**

| Issue | Solution |
|-------|----------|
| Dashboard won't load | Check WiFi connection to PostureCoach-AP |
| Camera feed black | Restart ESP32-CAM, check privacy mode |
| No distance reading | Check HC-SR04 wiring, ensure clear path |
| Button not working | Check debounce delay, verify wiring |
| LED not changing | Check posture score, verify LED wiring |
| Buzzer silent | Check volume, verify A0 connection |

### Firmware Updates

**Current version:** 1.0.0

**How to update:**
1. Download latest firmware from GitHub
2. Connect boards via USB (programming mode)
3. Upload new firmware via Arduino IDE
4. Restart boards
5. Verify functionality

**Check for updates:** https://github.com/hyoono/posture-coach-iot

---

## FAQ

### General Questions

**Q: Do I need internet for this to work?**
A: No! The system creates its own WiFi network (PostureCoach-AP). Everything works locally without internet.

**Q: Can multiple people use it?**
A: Yes, but one person at a time. Each session starts fresh when powered on.

**Q: How accurate is the posture detection?**
A: The system provides good estimates. It's designed as a reminder tool, not medical-grade analysis.

**Q: Can I use my regular WiFi too?**
A: Your device can only connect to one WiFi at a time. You'll need to switch networks to access the dashboard.

**Q: Is my video recorded or saved?**
A: No! Video is processed in real-time and immediately discarded. Nothing is stored or transmitted.

### Technical Questions

**Q: What's the camera range?**
A: Optimal: 1-2 feet away. Maximum: ~6 feet with reduced accuracy.

**Q: Can I change the WiFi password?**
A: Yes, edit the password in the ESP32-CAM firmware and re-upload.

**Q: Why port 192.168.4.1?**
A: This is the standard IP for ESP32 Access Point mode.

**Q: Can I add more sensors?**
A: Yes! The WEMOS D1 R1 has additional GPIO pins available for expansion.

**Q: What if I have multiple ESP32-CAM systems?**
A: Each should use a unique SSID to avoid conflicts.

### Usage Questions

**Q: How long should breaks be?**
A: The system enforces 5-minute breaks, but longer breaks (10-15 min) are beneficial.

**Q: Should I leave it on all the time?**
A: Yes, it's designed for continuous operation. Power off if not using for extended periods.

**Q: What if I get too many alerts?**
A: Lower the alert sensitivity in Settings or use Snooze button during focus time.

**Q: Can I use this while standing?**
A: Yes, but you'll need to adjust camera position and potentially recalibrate distance thresholds.

**Q: Does lighting affect accuracy?**
A: Yes, good lighting helps. Avoid backlighting (window behind you) for best results.

### Privacy Questions

**Q: Is anyone watching my camera feed?**
A: No! Everything is processed locally on the ESP32-CAM. No data leaves the device.

**Q: Can others connect to my PostureCoach-AP network?**
A: Yes if they have the password. Consider changing the default password for privacy.

**Q: What does Privacy Mode do?**
A: Stops camera capture and posture detection completely until toggled off.

---

## Support

### Getting Help

**Documentation:**
- [Troubleshooting Guide](09-TROUBLESHOOTING.md)
- [API Specification](06-API-SPECIFICATION.md)
- [Wiring Diagrams](04-WIRING-DIAGRAMS.md)

**Community:**
- GitHub Issues: https://github.com/hyoono/posture-coach-iot/issues
- Project Wiki: https://github.com/hyoono/posture-coach-iot/wiki

### Reporting Issues

When reporting problems, include:
1. What you were doing
2. What happened vs. what you expected
3. Error messages (if any)
4. Serial Monitor output (if available)
5. Photos of setup (for hardware issues)

---

## Tips for Best Results

### Ergonomic Setup

1. **Monitor Position:**
   - Top of screen at or below eye level
   - Arm's length distance
   - Directly in front (not to side)

2. **Chair Setup:**
   - Adjust height so feet flat on floor
   - Lumbar support for lower back
   - Armrests at desk height

3. **Keyboard/Mouse:**
   - Close to body
   - Elbows at 90°
   - Wrists neutral (not bent)

### Maximizing Effectiveness

✅ **DO:**
- Follow break reminders consistently
- Adjust settings to your needs
- Pay attention to alerts
- Combine with stretching routine
- Stay hydrated

❌ **DON'T:**
- Ignore repeated alerts
- Disable system during work
- Skip breaks regularly
- Maintain poor posture despite alerts

### Building Good Habits

**Week 1:** Focus on awareness
- Notice when alerts trigger
- Identify posture patterns
- Find comfortable upright position

**Week 2:** Active correction
- Respond immediately to alerts
- Take all recommended breaks
- Practice good posture consciously

**Week 3+:** Habit formation
- Good posture becomes natural
- Fewer alerts needed
- Maintain without thinking

---

## Conclusion

Congratulations! You now know how to use the Smart Posture Coach system effectively. Remember:

- **Consistency is key** - Use daily for best results
- **Listen to your body** - Don't override pain signals
- **Take breaks seriously** - They're essential for health
- **Adjust settings** - Customize to your needs

**Your posture impacts:**
- Long-term spinal health
- Energy levels
- Breathing efficiency
- Confidence and mood
- Work productivity

**Use this tool to build lasting healthy habits!**

---

**Need help?** See [Troubleshooting Guide](09-TROUBLESHOOTING.md)  
**Want to customize?** See [API Specification](06-API-SPECIFICATION.md)  
**Hardware questions?** See [Wiring Diagrams](04-WIRING-DIAGRAMS.md)

---

**Version:** 1.0.0  
**Last Updated:** December 2024  
**License:** MIT
