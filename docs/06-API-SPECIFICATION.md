# REST API Specification

## 📡 Complete HTTP API Documentation

This document describes all REST API endpoints provided by the ESP32-CAM for communication with WEMOS D1 R1 and web dashboard clients.

---

## Base URL

```
http://192.168.4.1
```

**WiFi Network:** PostureCoach-AP  
**Password:** posture123

---

## API Endpoints Overview

| Endpoint | Method | Purpose | Client |
|----------|--------|---------|--------|
| `/api/posture/current` | GET | Get real-time posture data | WEMOS, Dashboard |
| `/api/session/active` | GET | Get current session statistics | Dashboard |
| `/api/commands/pending` | GET | Poll for pending commands | WEMOS |
| `/api/sensor/distance` | POST | Send distance sensor reading | WEMOS |
| `/api/input/button` | POST | Send button press event | WEMOS |
| `/api/break/start` | POST | Trigger break mode | Dashboard, WEMOS |
| `/api/settings/update` | POST | Update system settings | Dashboard |
| `/stream` | GET | MJPEG camera stream | Dashboard |

---

## Detailed Endpoint Documentation

### 1. GET /api/posture/current

Get real-time posture analysis data.

**Authentication:** None  
**Rate Limit:** Poll every 3 seconds recommended  

**Request:**
```http
GET /api/posture/current HTTP/1.1
Host: 192.168.4.1
```

**Response:**
```json
{
  "score": 85,
  "headTilt": 12.5,
  "shoulderAlign": "good",
  "status": "good",
  "timestamp": 1234567890,
  "distance": 58
}
```

**Response Fields:**

| Field | Type | Description | Range/Values |
|-------|------|-------------|--------------|
| `score` | integer | Overall posture score | 0-100 |
| `headTilt` | float | Head forward tilt angle (degrees) | 0-45 |
| `shoulderAlign` | string | Shoulder alignment status | "good", "poor" |
| `status` | string | Overall posture status | "excellent", "good", "fair", "poor" |
| `timestamp` | integer | Milliseconds since boot | 0+ |
| `distance` | integer | Last known distance (cm) | 0-400 |

**Status Codes:**
- `200 OK` - Success
- `500 Internal Server Error` - Camera or processing error

**Example Usage (JavaScript):**
```javascript
const response = await fetch('http://192.168.4.1/api/posture/current');
const data = await response.json();
console.log('Posture Score:', data.score);
```

**Example Usage (WEMOS D1 R1):**
```cpp
HTTPClient http;
WiFiClient client;
http.begin(client, "http://192.168.4.1/api/posture/current");
int httpCode = http.GET();
if (httpCode == 200) {
  String payload = http.getString();
  // Parse JSON...
}
http.end();
```

---

### 2. GET /api/session/active

Get statistics for current monitoring session.

**Authentication:** None  
**Rate Limit:** Poll every 5 seconds recommended  

**Request:**
```http
GET /api/session/active HTTP/1.1
Host: 192.168.4.1
```

**Response:**
```json
{
  "duration": 3600,
  "averageScore": 82,
  "alertsTriggered": 5,
  "breaksTaken": 2,
  "startTime": 1234567890
}
```

**Response Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `duration` | integer | Session duration in seconds |
| `averageScore` | integer | Average posture score (0-100) |
| `alertsTriggered` | integer | Number of alerts during session |
| `breaksTaken` | integer | Number of breaks taken |
| `startTime` | integer | Session start timestamp (ms since boot) |

**Status Codes:**
- `200 OK` - Success

---

### 3. GET /api/commands/pending

Poll for commands from ESP32-CAM to WEMOS D1 R1.

**Authentication:** None  
**Rate Limit:** Poll every 3 seconds  
**Client:** WEMOS D1 R1 only  

**Request:**
```http
GET /api/commands/pending HTTP/1.1
Host: 192.168.4.1
```

**Response (Command Available):**
```json
{
  "command": "LED",
  "value": "YELLOW",
  "pattern": "SOLID",
  "duration": 0,
  "timestamp": 1234567890
}
```

**Response (No Command):**
```json
{
  "command": "NONE"
}
```

**Command Types:**

| Command | Value | Pattern | Duration | Description |
|---------|-------|---------|----------|-------------|
| `LED` | "RED", "GREEN", "BLUE", "YELLOW", "OFF" | "SOLID" | 0 | Set LED color |
| `BUZZER` | "" | "SUCCESS", "WARNING", "URGENT", "DISTANCE", "BREAK_REMINDER" | milliseconds | Play buzzer pattern |
| `BREAK` | "START", "END" | "" | seconds | Start/end break mode |
| `OLED` | "message text" | "" | 0 | Update OLED display |
| `NONE` | "" | "" | 0 | No pending commands |

**Status Codes:**
- `200 OK` - Success (command or no command)

**WEMOS D1 R1 Implementation:**
```cpp
void pollCommands() {
  HTTPClient http;
  WiFiClient client;
  http.begin(client, "http://192.168.4.1/api/commands/pending");
  
  int httpCode = http.GET();
  if (httpCode == 200) {
    String response = http.getString();
    StaticJsonDocument<256> doc;
    deserializeJson(doc, response);
    
    String command = doc["command"];
    if (command == "LED") {
      String color = doc["value"];
      executeLEDCommand(color);
    } else if (command == "BUZZER") {
      String pattern = doc["pattern"];
      playBuzzerPattern(pattern);
    }
  }
  http.end();
}
```

---

### 4. POST /api/sensor/distance

Send distance sensor reading from WEMOS D1 R1 to ESP32-CAM.

**Authentication:** None  
**Rate Limit:** Send every 2 seconds  
**Client:** WEMOS D1 R1 only  

**Request:**
```http
POST /api/sensor/distance HTTP/1.1
Host: 192.168.4.1
Content-Type: application/json

{
  "distance": 58,
  "unit": "cm",
  "timestamp": 1234567890
}
```

**Request Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `distance` | integer | Yes | Distance in centimeters (0-400) |
| `unit` | string | Yes | Measurement unit ("cm") |
| `timestamp` | integer | Yes | Timestamp (ms since boot) |

**Response:**
```json
{
  "status": "ok"
}
```

**Status Codes:**
- `200 OK` - Distance received successfully
- `400 Bad Request` - Invalid JSON or missing fields

**WEMOS D1 R1 Implementation:**
```cpp
void sendSensorData() {
  HTTPClient http;
  WiFiClient client;
  
  http.begin(client, "http://192.168.4.1/api/sensor/distance");
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<256> doc;
  doc["distance"] = distance;
  doc["unit"] = "cm";
  doc["timestamp"] = millis();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  http.end();
}
```

---

### 5. POST /api/input/button

Send button press event from WEMOS D1 R1 to ESP32-CAM.

**Authentication:** None  
**Client:** WEMOS D1 R1 only  

**Request:**
```http
POST /api/input/button HTTP/1.1
Host: 192.168.4.1
Content-Type: application/json

{
  "button": 1,
  "action": "PRESSED",
  "timestamp": 1234567890
}
```

**Request Fields:**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `button` | integer | Yes | Button number (1, 2, or 3) |
| `action` | string | Yes | Action type ("PRESSED" or "RELEASED") |
| `timestamp` | integer | Yes | Timestamp (ms since boot) |

**Button Mappings:**
- Button 1: Manual break start
- Button 2: Snooze alerts (5 minutes)
- Button 3: Toggle privacy mode

**Response (Button 1):**
```json
{
  "status": "break_started"
}
```

**Response (Button 2):**
```json
{
  "status": "snoozed"
}
```

**Response (Button 3):**
```json
{
  "status": "privacy_enabled"
}
```
or
```json
{
  "status": "privacy_disabled"
}
```

**Status Codes:**
- `200 OK` - Button action processed
- `400 Bad Request` - Invalid JSON or button number

---

### 6. POST /api/break/start

Trigger break mode remotely (from web dashboard or WEMOS button).

**Authentication:** None  
**Client:** Dashboard or WEMOS D1 R1  

**Request:**
```http
POST /api/break/start HTTP/1.1
Host: 192.168.4.1
Content-Type: application/json
```

**Response:**
```json
{
  "status": "break_started"
}
```

**Break Mode Effects:**
- Stops posture monitoring for 5 minutes
- Increments break counter
- Sends BREAK command to WEMOS D1 R1
- Updates session statistics

**Status Codes:**
- `200 OK` - Break started successfully

**JavaScript Example:**
```javascript
async function takeBreak() {
  const response = await fetch('http://192.168.4.1/api/break/start', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    }
  });
  const result = await response.json();
  console.log(result.status);
}
```

---

### 7. POST /api/settings/update

Update system configuration settings.

**Authentication:** None  
**Client:** Dashboard  

**Request:**
```http
POST /api/settings/update HTTP/1.1
Host: 192.168.4.1
Content-Type: application/json

{
  "alertSensitivity": 75,
  "breakInterval": 45,
  "minDistance": 50,
  "maxDistance": 70
}
```

**Request Fields:**

| Field | Type | Required | Description | Range |
|-------|------|----------|-------------|-------|
| `alertSensitivity` | integer | No | Alert sensitivity percentage | 0-100 |
| `breakInterval` | integer | No | Break interval (minutes) | 30, 45, 60 |
| `minDistance` | integer | No | Minimum optimal distance (cm) | 30-70 |
| `maxDistance` | integer | No | Maximum optimal distance (cm) | 50-100 |

**Response:**
```json
{
  "status": "updated"
}
```

**Status Codes:**
- `200 OK` - Settings updated successfully
- `400 Bad Request` - Invalid values

---

### 8. GET /stream

MJPEG camera live stream endpoint.

**Authentication:** None  
**Rate Limit:** Continuous stream (10 FPS)  
**Client:** Web dashboard  

**Request:**
```http
GET /stream HTTP/1.1
Host: 192.168.4.1
```

**Response:**
```
HTTP/1.1 200 OK
Content-Type: multipart/x-mixed-replace; boundary=frame

--frame
Content-Type: image/jpeg
Content-Length: [size]

[JPEG image data]
--frame
Content-Type: image/jpeg
Content-Length: [size]

[JPEG image data]
...
```

**Stream Properties:**
- Format: MJPEG (Motion JPEG)
- Frame Rate: ~10 FPS
- Resolution: 640x480 (VGA) or 320x240 (QVGA)
- Quality: Adjustable (default: 10)

**HTML Usage:**
```html
<img src="http://192.168.4.1/stream" width="640" height="480" alt="Camera Feed">
```

**JavaScript Usage:**
```javascript
const cameraStream = document.getElementById('cameraStream');
cameraStream.src = 'http://192.168.4.1/stream';
cameraStream.onerror = () => {
  console.error('Stream failed to load');
};
```

**Privacy Mode:** When privacy mode is enabled, stream pauses until disabled.

---

## Communication Flow Diagrams

### WEMOS D1 R1 → ESP32-CAM (Sensor Data)

```
┌─────────────┐                      ┌─────────────┐
│  WEMOS D1   │                      │ ESP32-CAM   │
│     R1      │                      │             │
└──────┬──────┘                      └──────┬──────┘
       │                                    │
       │ POST /api/sensor/distance          │
       │ {"distance": 58, "unit": "cm"}     │
       ├───────────────────────────────────>│
       │                                    │
       │         200 OK {"status": "ok"}    │
       │<───────────────────────────────────┤
       │                                    │
       │ [Updates posture score]            │
       │                                    │
```

### ESP32-CAM → WEMOS D1 R1 (Command Polling)

```
┌─────────────┐                      ┌─────────────┐
│  WEMOS D1   │                      │ ESP32-CAM   │
│     R1      │                      │             │
└──────┬──────┘                      └──────┬──────┘
       │                                    │
       │ GET /api/commands/pending          │
       ├───────────────────────────────────>│
       │                                    │
       │ 200 OK {"command": "LED",          │
       │         "value": "RED"}            │
       │<───────────────────────────────────┤
       │                                    │
       │ [Executes LED command]             │
       │                                    │
```

### Dashboard → ESP32-CAM (Data Polling)

```
┌─────────────┐                      ┌─────────────┐
│  Dashboard  │                      │ ESP32-CAM   │
│  (Browser)  │                      │             │
└──────┬──────┘                      └──────┬──────┘
       │                                    │
       │ GET /api/posture/current           │
       ├───────────────────────────────────>│
       │                                    │
       │ 200 OK {"score": 85, ...}          │
       │<───────────────────────────────────┤
       │                                    │
       │ [Updates UI]                       │
       │                                    │
```

---

## Error Handling

### Standard Error Response

```json
{
  "error": "Error message",
  "code": "ERROR_CODE"
}
```

### Common Error Codes

| HTTP Code | Error Code | Description |
|-----------|-----------|-------------|
| 400 | INVALID_JSON | Malformed JSON in request body |
| 400 | MISSING_FIELD | Required field missing |
| 400 | INVALID_VALUE | Field value out of range |
| 404 | NOT_FOUND | Endpoint not found |
| 500 | INTERNAL_ERROR | Server processing error |
| 503 | SERVICE_UNAVAILABLE | Camera or service unavailable |

---

## Best Practices

### For WEMOS D1 R1

1. **Polling Intervals:**
   - Distance readings: Every 2 seconds
   - Posture data: Every 3 seconds
   - Commands: Every 3 seconds

2. **Error Handling:**
   - Implement retry logic (max 3 attempts)
   - Set HTTP timeout to 5 seconds
   - Handle disconnections gracefully

3. **Memory Management:**
   - Use StaticJsonDocument for fixed-size JSON
   - Limit payload sizes to 512 bytes
   - Free HTTP client resources after use

### For Web Dashboard

1. **Polling Strategy:**
   - Posture data: Every 3 seconds
   - Session data: Every 5 seconds
   - Use AbortController for fetch timeouts

2. **Stream Handling:**
   - Use error handlers for stream failures
   - Implement automatic reconnection
   - Display connection status to user

3. **User Experience:**
   - Show loading states during API calls
   - Display error messages clearly
   - Cache last known good data

---

## Testing API Endpoints

### Using curl

```bash
# Get posture data
curl http://192.168.4.1/api/posture/current

# Send distance reading
curl -X POST http://192.168.4.1/api/sensor/distance \
  -H "Content-Type: application/json" \
  -d '{"distance": 58, "unit": "cm", "timestamp": 123456}'

# Start break
curl -X POST http://192.168.4.1/api/break/start

# Update settings
curl -X POST http://192.168.4.1/api/settings/update \
  -H "Content-Type: application/json" \
  -d '{"alertSensitivity": 80, "breakInterval": 45}'
```

### Using Postman

1. Create new request
2. Set method and URL
3. Add headers: `Content-Type: application/json`
4. Add JSON body for POST requests
5. Send and inspect response

---

## Rate Limiting & Performance

### Recommended Polling Rates

| Client | Endpoint | Interval | Reason |
|--------|----------|----------|--------|
| WEMOS | /api/commands/pending | 3s | Balance responsiveness and load |
| WEMOS | /api/sensor/distance | 2s | Frequent distance updates |
| Dashboard | /api/posture/current | 3s | Real-time UI updates |
| Dashboard | /api/session/active | 5s | Less critical data |

### Performance Considerations

- ESP32-CAM can handle 3-4 simultaneous connections
- MJPEG stream consumes ~500KB/s bandwidth
- API responses typically < 1KB
- Average response time: 50-200ms

---

## Security Notes

⚠️ **This is a local development system without authentication.**

For production use, consider:
- HTTPS/TLS encryption
- API key authentication
- Rate limiting per client
- CORS policy configuration
- Input validation and sanitization

---

## Changelog

### Version 1.0.0 (Current)
- Initial API specification
- 8 REST endpoints
- MJPEG streaming support
- JSON request/response format

---

## Related Documentation

- [System Architecture](02-SYSTEM-ARCHITECTURE.md)
- [Software Architecture](05-SOFTWARE-ARCHITECTURE.md)
- [User Manual](08-USER-MANUAL.md)
- [Troubleshooting](09-TROUBLESHOOTING.md)
