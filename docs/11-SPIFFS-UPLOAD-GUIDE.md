# SPIFFS Upload Guide for ESP32-CAM

Complete guide for uploading web dashboard files to ESP32-CAM's SPIFFS file system.

---

## Overview

The ESP32-CAM needs three files uploaded to SPIFFS:
- `/index.html` - Main dashboard page
- `/style.css` - Stylesheet
- `/script.js` - JavaScript code

These files are located in: `esp32-cam/data/`

---

## Method 1: Arduino IDE 1.8.x (Legacy - Recommended)

### Installation
1. Download Arduino IDE 1.8.19 from: https://www.arduino.cc/en/software
2. Install ESP32 board support (if not already installed)
3. Download ESP32 Sketch Data Upload Plugin:
   - Visit: https://github.com/me-no-dev/arduino-esp32fs-plugin/releases
   - Download: `ESP32FS-1.1.zip` (or latest version)
4. Extract the ZIP file
5. Create folders: `Arduino/tools/ESP32FS/tool/`
6. Place `esp32fs.jar` in the `tool` folder

### Upload Steps
1. Open `esp32-cam.ino` in Arduino IDE 1.8.x
2. Ensure `data` folder is in same directory as `esp32-cam.ino`
3. Connect ESP32-CAM via USB-to-Serial adapter
4. Select correct Board: `AI Thinker ESP32-CAM`
5. Select correct Port
6. Go to: **Tools → ESP32 Sketch Data Upload**
7. Wait for upload to complete (~30 seconds)
8. Reset ESP32-CAM

**Note:** This is the most reliable method but requires Arduino IDE 1.8.x.

---

## Method 2: PlatformIO (Professional)

### Setup
Create `platformio.ini` in project root:

```ini
[env:esp32cam]
platform = espressif32
board = esp32cam
framework = arduino
board_build.partitions = min_spiffs.csv
board_build.filesystem = littlefs

lib_deps =
    me-no-dev/ESP Async WebServer@^1.2.3
    me-no-dev/AsyncTCP@^1.1.1
    bblanchon/ArduinoJson@^6.21.3

upload_speed = 921600
monitor_speed = 115200
```

### Upload Steps
1. Place `data` folder in project root
2. Open terminal in project directory
3. Run: `pio run --target uploadfs`
4. Wait for completion
5. Upload firmware: `pio run --target upload`

---

## Method 3: esptool.py + mkspiffs (Advanced)

### Prerequisites
```bash
# Install Python tools
pip install esptool

# Download mkspiffs
# Windows: https://github.com/igrr/mkspiffs/releases
# Linux: sudo apt install mkspiffs
# macOS: brew install mkspiffs
```

### Create SPIFFS Image
```bash
# Navigate to esp32-cam directory
cd esp32-cam

# Create SPIFFS image (0x190000 = 1.6MB)
mkspiffs -c data -b 4096 -p 256 -s 0x190000 spiffs.bin

# On Windows, full command:
mkspiffs.exe -c data -b 4096 -p 256 -s 0x190000 spiffs.bin
```

### Upload to ESP32-CAM
```bash
# Find COM port (COM3, COM4, etc. on Windows; /dev/ttyUSB0 on Linux)
# Replace COM3 with your port

# Upload SPIFFS image at address 0x290000
esptool.py --chip esp32 --port COM3 --baud 460800 ^
    write_flash -z 0x290000 spiffs.bin

# On Linux/Mac:
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 \
    write_flash -z 0x290000 spiffs.bin
```

### Partition Addresses
- Default partition scheme: `min_spiffs.csv`
- SPIFFS starts at: `0x290000`
- Size: 1.6 MB (0x190000 bytes)

---

## Method 4: Arduino IDE 2.x with ESP32FS (Experimental)

Arduino IDE 2.3.x has compatibility issues with the old plugin. Try this workaround:

### Option A: Manual Installation
1. Download ESP32FS plugin for IDE 2.x (if available)
2. Extract to: `~/.arduino15/packages/esp32/tools/`
3. Restart Arduino IDE
4. Check Tools menu for upload option

### Option B: Use Arduino-CLI
```bash
# Install Arduino CLI
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Create sketch
arduino-cli sketch new esp32cam-project

# Upload filesystem
arduino-cli upload -b esp32:esp32:esp32cam --input-dir data esp32cam-project
```

---

## Method 5: Web-Based Upload (Alternative)

If SPIFFS upload fails, you can serve files from SD card or modify code to embed HTML:

### Option A: Embed HTML in Code
```cpp
// In esp32-cam.ino, replace SPIFFS.send() with:
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Posture Coach</title>
    <!-- Full HTML here -->
</head>
<body>
    <!-- Dashboard content -->
</body>
</html>
)rawliteral");
});
```

### Option B: SD Card
1. Copy files to SD card root
2. Modify code to use SD library instead of SPIFFS
3. Insert SD card in ESP32-CAM slot

---

## Verification

After uploading SPIFFS, check Serial Monitor (115200 baud):

```
ESP32-CAM Posture Coach Starting...
========================================

SPIFFS mounted successfully

SPIFFS Files:
  /index.html (4532 bytes)
  /style.css (3421 bytes)
  /script.js (2876 bytes)
```

If no files listed, upload failed. Try another method.

---

## Troubleshooting

### Files Not Showing Up

**Problem:** Serial Monitor shows "SPIFFS Files:" but no files listed

**Solutions:**
1. Verify `data` folder exists in same directory as `esp32-cam.ino`
2. Check file names: must be `index.html`, `style.css`, `script.js` (lowercase)
3. Ensure no subdirectories in `data` folder (files should be at root)
4. Try different upload method
5. Increase SPIFFS partition size in Tools → Partition Scheme

### Plugin Not Appearing in Tools Menu

**Problem:** "ESP32 Sketch Data Upload" not in Arduino IDE 2.x

**Solution:** Arduino IDE 2.x has compatibility issues. Use:
- Arduino IDE 1.8.19 (recommended)
- PlatformIO (professional alternative)
- esptool.py + mkspiffs (command-line)

### Upload Fails with "Timeout"

**Problem:** esptool.py times out during upload

**Solutions:**
1. Put ESP32-CAM in flash mode:
   - Connect GPIO0 to GND
   - Press reset button
   - Remove GPIO0-GND connection after upload starts
2. Lower baud rate: use `--baud 115200` instead of `460800`
3. Try different USB cable (some cables are power-only)
4. Use external 5V power supply during upload

### Wrong Partition Address

**Problem:** Files uploaded but not accessible

**Solution:** Check partition scheme:
```bash
# List partitions
esptool.py --port COM3 read_flash 0x8000 0xC00 partitions.bin
python -m esptool partition_table partitions.bin
```

Common partition schemes:
- **Default:** SPIFFS at 0x290000
- **Minimal SPIFFS:** SPIFFS at 0x310000
- **No OTA:** SPIFFS at 0x200000

Match upload address to your partition scheme in Tools → Partition Scheme.

### SPIFFS Mount Failed

**Problem:** Serial Monitor shows "SPIFFS Mount Failed"

**Solutions:**
1. Format SPIFFS: `SPIFFS.format()` in setup() (then re-upload files)
2. Use LittleFS instead:
   ```cpp
   #include <LittleFS.h>
   // Replace SPIFFS with LittleFS throughout code
   LittleFS.begin(true);
   ```
3. Check partition table has SPIFFS partition

---

## Recommended Workflow

For best results, follow this order:

1. **First Time Setup:**
   - Use Arduino IDE 1.8.19 + ESP32FS plugin (easiest)
   - Upload firmware first
   - Then upload SPIFFS data

2. **Development:**
   - Use PlatformIO (automatic FS upload)
   - Changes to HTML/CSS/JS: `pio run --target uploadfs`
   - Changes to firmware: `pio run --target upload`

3. **Production:**
   - Pre-build SPIFFS image with mkspiffs
   - Flash both firmware and SPIFFS in one step
   - Use OTA updates for web files

---

## File Size Limits

ESP32-CAM SPIFFS capacity:
- **Default partition:** 1.5 MB
- **Minimal SPIFFS:** ~0.2 MB
- **No OTA partition:** 3 MB

Current web files:
- `index.html`: ~4.5 KB
- `style.css`: ~3.4 KB
- `script.js`: ~2.9 KB
- **Total:** ~11 KB (plenty of space)

---

## Alternative: LittleFS

LittleFS is more reliable than SPIFFS. To use LittleFS:

1. Change includes:
```cpp
#include <LittleFS.h>
#define SPIFFS LittleFS
```

2. Upload with PlatformIO:
```ini
board_build.filesystem = littlefs
```

3. Upload with esptool:
```bash
mklittlefs -c data -b 4096 -p 256 -s 0x190000 littlefs.bin
esptool.py write_flash 0x290000 littlefs.bin
```

---

## Quick Reference

| Method | Difficulty | Reliability | Arduino 2.x |
|--------|-----------|-------------|-------------|
| Arduino 1.8.x + Plugin | Easy | High | No |
| PlatformIO | Medium | Very High | Yes |
| esptool + mkspiffs | Hard | High | Yes |
| Embedded HTML | Easy | High | Yes |

**Recommendation:** Use Arduino IDE 1.8.19 for simplicity, or PlatformIO for professional development.

---

## Support

If still having issues:
1. Check Serial Monitor output at 115200 baud
2. Verify `data` folder structure matches repository
3. Try different upload method
4. Consider using embedded HTML (no SPIFFS needed)
5. Open GitHub issue with Serial Monitor output

---

**Last Updated:** 2024-11-26  
**Arduino IDE Version:** 1.8.19 (recommended), 2.3.6 (experimental)  
**ESP32 Board Package:** 2.0.0+
