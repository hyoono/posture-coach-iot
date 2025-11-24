/*
 * ESP32-CAM Posture Coach - Main Firmware
 * 
 * This firmware runs on the ESP32-CAM and acts as:
 * - WiFi Access Point (PostureCoach-AP)
 * - Web Server (serving dashboard from SPIFFS)
 * - REST API Server (for WEMOS D1 R1 communication)
 * - Camera Server (MJPEG streaming)
 * - Posture Detection Engine
 * 
 * Board: AI Thinker ESP32-CAM
 * Partition: Minimal SPIFFS (1.9MB APP / 190KB SPIFFS)
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "esp_camera.h"
#include "esp_timer.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

// ============================================================================
// CONFIGURATION
// ============================================================================

// WiFi Access Point Configuration
const char* AP_SSID = "PostureCoach-AP";
const char* AP_PASSWORD = "posture123";
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Camera Pin Configuration (AI-Thinker ESP32-CAM)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Posture Detection Thresholds
#define POSTURE_EXCELLENT  90
#define POSTURE_GOOD       75
#define POSTURE_FAIR       60
#define HEAD_TILT_MAX      45
#define HEAD_TILT_OK       15
#define DISTANCE_MIN       50
#define DISTANCE_MAX       70
#define BREAK_INTERVAL     2700000  // 45 minutes in milliseconds
#define BREAK_DURATION     300000   // 5 minutes in milliseconds

// FreeRTOS Configuration
#define STACK_SIZE_POSTURE   8192
#define STACK_SIZE_STREAM    4096
#define STACK_SIZE_WIFI      3072
#define STACK_SIZE_HTTP      6144
#define STACK_SIZE_API       4096
#define STACK_SIZE_ANALYTICS 4096

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Web Server
AsyncWebServer server(80);

// FreeRTOS Handles
TaskHandle_t postureTaskHandle = NULL;
TaskHandle_t streamTaskHandle = NULL;
TaskHandle_t wifiTaskHandle = NULL;
TaskHandle_t httpTaskHandle = NULL;
TaskHandle_t apiTaskHandle = NULL;
TaskHandle_t analyticsTaskHandle = NULL;

// Semaphores & Mutexes
SemaphoreHandle_t cameraMutex;
SemaphoreHandle_t scoreMutex;
SemaphoreHandle_t spiffsMutex;

// Queues
QueueHandle_t commandQueue;
QueueHandle_t sensorDataQueue;

// Shared State (Protected by Mutexes)
struct PostureData {
  int score;
  float headTilt;
  String shoulderAlign;
  String status;
  unsigned long timestamp;
} currentPosture;

struct SessionData {
  unsigned long startTime;
  unsigned long duration;
  int averageScore;
  int alertsTriggered;
  int breaksTaken;
  int totalReadings;
  int scoreSum;
} currentSession;

struct SensorReading {
  int distance;
  unsigned long timestamp;
} lastSensorReading;

struct CommandData {
  String command;
  String value;
  String pattern;
  int duration;
} pendingCommand;

// Flags
bool cameraInitialized = false;
bool privacyMode = false;
bool breakMode = false;
unsigned long lastBreakTime = 0;
unsigned long breakStartTime = 0;

// ============================================================================
// CAMERA INITIALIZATION
// ============================================================================

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Init with high specs for detection
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;  // 640x480
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QVGA; // 320x240
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  // Sensor settings for better face detection
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  
  Serial.println("Camera initialized successfully");
  cameraInitialized = true;
  return true;
}

// ============================================================================
// WIFI ACCESS POINT SETUP
// ============================================================================

void setupWiFiAP() {
  Serial.println("Configuring WiFi Access Point...");
  
  // Configure AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  
  // Start AP
  bool apStarted = WiFi.softAP(AP_SSID, AP_PASSWORD, 6, 0, 4);
  
  if(apStarted) {
    Serial.println("WiFi AP started successfully!");
    Serial.print("AP SSID: ");
    Serial.println(AP_SSID);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.print("AP MAC: ");
    Serial.println(WiFi.softAPmacAddress());
  } else {
    Serial.println("WiFi AP failed to start!");
  }
}

// ============================================================================
// POSTURE DETECTION ALGORITHM
// ============================================================================

int calculatePostureScore(camera_fb_t * fb) {
  // Simplified posture detection algorithm
  // In production, use face detection library like esp-face or TensorFlow Lite
  
  int baseScore = 100;
  int penalty = 0;
  
  // Simulate head tilt detection (placeholder)
  // In real implementation: use face landmark detection
  float simulatedHeadTilt = random(0, 30);  // 0-30 degrees
  
  if(simulatedHeadTilt > HEAD_TILT_OK) {
    penalty += (simulatedHeadTilt - HEAD_TILT_OK);
    if(penalty > 30) penalty = 30;
  }
  
  // Shoulder alignment check (placeholder)
  bool shoulderSlouchDetected = (random(0, 100) < 20);  // 20% chance
  if(shoulderSlouchDetected) {
    penalty += 25;
  }
  
  // Distance penalty (from sensor data)
  if(lastSensorReading.distance > 0) {
    if(lastSensorReading.distance < DISTANCE_MIN) {
      penalty += (DISTANCE_MIN - lastSensorReading.distance) / 2;
    } else if(lastSensorReading.distance > DISTANCE_MAX) {
      penalty += (lastSensorReading.distance - DISTANCE_MAX) / 3;
    }
    if(penalty > 50) penalty = 50;  // Cap distance penalty
  }
  
  // Time penalty (sitting too long)
  if(millis() - lastBreakTime > BREAK_INTERVAL) {
    penalty += 15;
  }
  
  int finalScore = baseScore - penalty;
  if(finalScore < 0) finalScore = 0;
  
  // Update posture data
  if(xSemaphoreTake(scoreMutex, portMAX_DELAY)) {
    currentPosture.score = finalScore;
    currentPosture.headTilt = simulatedHeadTilt;
    currentPosture.shoulderAlign = shoulderSlouchDetected ? "poor" : "good";
    
    if(finalScore >= POSTURE_EXCELLENT) currentPosture.status = "excellent";
    else if(finalScore >= POSTURE_GOOD) currentPosture.status = "good";
    else if(finalScore >= POSTURE_FAIR) currentPosture.status = "fair";
    else currentPosture.status = "poor";
    
    currentPosture.timestamp = millis();
    xSemaphoreGive(scoreMutex);
  }
  
  // Update session statistics
  currentSession.totalReadings++;
  currentSession.scoreSum += finalScore;
  currentSession.averageScore = currentSession.scoreSum / currentSession.totalReadings;
  
  // Generate command for WEMOS based on score
  CommandData cmd;
  if(finalScore >= POSTURE_EXCELLENT) {
    cmd.command = "LED";
    cmd.value = "GREEN";
    cmd.pattern = "SOLID";
  } else if(finalScore >= POSTURE_GOOD) {
    cmd.command = "LED";
    cmd.value = "BLUE";
    cmd.pattern = "SOLID";
  } else if(finalScore >= POSTURE_FAIR) {
    cmd.command = "LED";
    cmd.value = "YELLOW";
    cmd.pattern = "SOLID";
    cmd.command = "BUZZER";
    cmd.pattern = "WARNING";
    cmd.duration = 500;
    currentSession.alertsTriggered++;
  } else {
    cmd.command = "LED";
    cmd.value = "RED";
    cmd.pattern = "SOLID";
    cmd.command = "BUZZER";
    cmd.pattern = "URGENT";
    cmd.duration = 1000;
    currentSession.alertsTriggered++;
  }
  
  xQueueSend(commandQueue, &cmd, 0);
  
  return finalScore;
}

// ============================================================================
// FREERTOS TASK 1: POSTURE DETECTION
// ============================================================================

void postureDetectionTask(void *parameter) {
  Serial.println("Posture Detection Task started on Core 0");
  
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(2000);  // 2 seconds = 0.5 FPS
  
  while(1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    
    if(!cameraInitialized || privacyMode || breakMode) {
      continue;
    }
    
    // Capture frame
    if(xSemaphoreTake(cameraMutex, pdMS_TO_TICKS(1000))) {
      camera_fb_t * fb = esp_camera_fb_get();
      
      if(fb) {
        // Run posture detection
        int score = calculatePostureScore(fb);
        
        Serial.printf("Posture Score: %d, Status: %s\n", 
                      currentPosture.score, 
                      currentPosture.status.c_str());
        
        esp_camera_fb_return(fb);
      } else {
        Serial.println("Camera capture failed");
      }
      
      xSemaphoreGive(cameraMutex);
    }
  }
}

// ============================================================================
// MJPEG STREAMING HANDLER (AsyncWebServer compatible)
// ============================================================================

class AsyncJpegStreamResponse: public AsyncAbstractResponse {
  private:
    camera_fb_t * _fb;
    size_t _index;
  public:
    AsyncJpegStreamResponse() {
      _callback = nullptr;
      _code = 200;
      _contentLength = 0;
      _contentType = "multipart/x-mixed-replace; boundary=frame";
      _sendContentLength = false;
      _chunked = true;
      _index = 0;
      _fb = NULL;
    }
    
    ~AsyncJpegStreamResponse() {
      if (_fb) {
        esp_camera_fb_return(_fb);
        _fb = NULL;
      }
    }
    
    bool _sourceValid() const {
      return cameraInitialized && !privacyMode;
    }
    
    size_t _fillBuffer(uint8_t *buf, size_t maxLen) {
      if (!_sourceValid()) {
        return 0;
      }
      
      size_t ret = 0;
      
      if (_fb == NULL) {
        if (xSemaphoreTake(cameraMutex, pdMS_TO_TICKS(100))) {
          _fb = esp_camera_fb_get();
          xSemaphoreGive(cameraMutex);
        }
        
        if (_fb == NULL) {
          return 0;
        }
        
        _index = 0;
        
        // Send boundary and headers
        size_t hlen = snprintf((char *)buf, maxLen,
          "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
          _fb->len);
        ret = hlen;
        return ret;
      }
      
      // Send JPEG data
      if (_index < _fb->len) {
        size_t will_copy = (_fb->len - _index) < maxLen ? (_fb->len - _index) : maxLen;
        memcpy(buf, _fb->buf + _index, will_copy);
        _index += will_copy;
        ret = will_copy;
      }
      
      // Finished sending this frame
      if (_index >= _fb->len) {
        esp_camera_fb_return(_fb);
        _fb = NULL;
        _index = 0;
        
        // Add small delay between frames (10 FPS)
        vTaskDelay(pdMS_TO_TICKS(100));
      }
      
      return ret;
    }
};

// ============================================================================
// REST API ENDPOINTS
// ============================================================================

// GET /api/posture/current
void handleGetPostureCurrent(AsyncWebServerRequest *request) {
  DynamicJsonDocument doc(512);
  
  if(xSemaphoreTake(scoreMutex, pdMS_TO_TICKS(100))) {
    doc["score"] = currentPosture.score;
    doc["headTilt"] = currentPosture.headTilt;
    doc["shoulderAlign"] = currentPosture.shoulderAlign;
    doc["status"] = currentPosture.status;
    doc["timestamp"] = currentPosture.timestamp;
    doc["distance"] = lastSensorReading.distance;
    xSemaphoreGive(scoreMutex);
  }
  
  String response;
  serializeJson(doc, response);
  request->send(200, "application/json", response);
}

// GET /api/session/active
void handleGetSessionActive(AsyncWebServerRequest *request) {
  DynamicJsonDocument doc(512);
  
  currentSession.duration = (millis() - currentSession.startTime) / 1000;
  
  doc["duration"] = currentSession.duration;
  doc["averageScore"] = currentSession.averageScore;
  doc["alertsTriggered"] = currentSession.alertsTriggered;
  doc["breaksTaken"] = currentSession.breaksTaken;
  doc["startTime"] = currentSession.startTime;
  
  String response;
  serializeJson(doc, response);
  request->send(200, "application/json", response);
}

// POST /api/sensor/distance
void handlePostSensorDistance(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, data, len);
  
  if(error) {
    request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  lastSensorReading.distance = doc["distance"];
  lastSensorReading.timestamp = millis();
  
  Serial.printf("Distance received: %d cm\n", lastSensorReading.distance);
  
  request->send(200, "application/json", "{\"status\":\"ok\"}");
}

// POST /api/input/button
void handlePostInputButton(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, data, len);
  
  if(error) {
    request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  int button = doc["button"];
  String action = doc["action"];
  
  Serial.printf("Button %d: %s\n", button, action.c_str());
  
  if(button == 1 && action == "PRESSED") {
    // Manual break
    breakMode = true;
    breakStartTime = millis();
    lastBreakTime = millis();
    currentSession.breaksTaken++;
    
    CommandData cmd;
    cmd.command = "BREAK";
    cmd.value = "START";
    cmd.duration = BREAK_DURATION / 1000;
    xQueueSend(commandQueue, &cmd, 0);
    
    request->send(200, "application/json", "{\"status\":\"break_started\"}");
  } else if(button == 2 && action == "PRESSED") {
    // Snooze
    request->send(200, "application/json", "{\"status\":\"snoozed\"}");
  } else if(button == 3 && action == "PRESSED") {
    // Privacy mode toggle
    privacyMode = !privacyMode;
    String status = privacyMode ? "enabled" : "disabled";
    request->send(200, "application/json", "{\"status\":\"privacy_" + status + "\"}");
  } else {
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  }
}

// GET /api/commands/pending
void handleGetCommandsPending(AsyncWebServerRequest *request) {
  CommandData cmd;
  
  if(xQueueReceive(commandQueue, &cmd, 0) == pdTRUE) {
    DynamicJsonDocument doc(256);
    doc["command"] = cmd.command;
    doc["value"] = cmd.value;
    doc["pattern"] = cmd.pattern;
    doc["duration"] = cmd.duration;
    doc["timestamp"] = millis();
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  } else {
    request->send(200, "application/json", "{\"command\":\"NONE\"}");
  }
}

// POST /api/break/start
void handlePostBreakStart(AsyncWebServerRequest *request) {
  breakMode = true;
  breakStartTime = millis();
  lastBreakTime = millis();
  currentSession.breaksTaken++;
  
  CommandData cmd;
  cmd.command = "BREAK";
  cmd.value = "START";
  cmd.duration = BREAK_DURATION / 1000;
  xQueueSend(commandQueue, &cmd, 0);
  
  request->send(200, "application/json", "{\"status\":\"break_started\"}");
}

// POST /api/settings/update
void handlePostSettingsUpdate(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, data, len);
  
  if(error) {
    request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  // Update settings (implement as needed)
  
  request->send(200, "application/json", "{\"status\":\"updated\"}");
}

// ============================================================================
// WEB SERVER SETUP
// ============================================================================

void setupWebServer() {
  // Serve static files from SPIFFS
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "application/javascript");
  });
  
  // MJPEG Stream endpoint (integrated into main web server)
  server.on("/stream", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncJpegStreamResponse *response = new AsyncJpegStreamResponse();
    request->send(response);
  });
  
  // API Endpoints
  server.on("/api/posture/current", HTTP_GET, handleGetPostureCurrent);
  server.on("/api/session/active", HTTP_GET, handleGetSessionActive);
  server.on("/api/commands/pending", HTTP_GET, handleGetCommandsPending);
  
  server.on("/api/sensor/distance", HTTP_POST, [](AsyncWebServerRequest *request){} , NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
              handlePostSensorDistance(request, data, len, index, total);
            });
  
  server.on("/api/input/button", HTTP_POST, [](AsyncWebServerRequest *request){} , NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
              handlePostInputButton(request, data, len, index, total);
            });
  
  server.on("/api/break/start", HTTP_POST, handlePostBreakStart);
  
  server.on("/api/settings/update", HTTP_POST, [](AsyncWebServerRequest *request){} , NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
              handlePostSettingsUpdate(request, data, len, index, total);
            });
  
  // 404 handler
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not Found");
  });
  
  server.begin();
  Serial.println("HTTP server started on port 80");
  Serial.println("  Dashboard: http://192.168.4.1/");
  Serial.println("  Live Stream: http://192.168.4.1/stream");
}

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n========================================");
  Serial.println("ESP32-CAM Posture Coach Starting...");
  Serial.println("========================================\n");
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.println("SPIFFS mounted successfully");
  
  // Initialize Camera
  if(!initCamera()) {
    Serial.println("Camera initialization failed!");
    return;
  }
  
  // Setup WiFi AP
  setupWiFiAP();
  
  // Create Mutexes
  cameraMutex = xSemaphoreCreateMutex();
  scoreMutex = xSemaphoreCreateMutex();
  spiffsMutex = xSemaphoreCreateMutex();
  
  // Create Queues
  commandQueue = xQueueCreate(10, sizeof(CommandData));
  sensorDataQueue = xQueueCreate(10, sizeof(SensorReading));
  
  // Initialize session
  currentSession.startTime = millis();
  currentSession.duration = 0;
  currentSession.averageScore = 0;
  currentSession.alertsTriggered = 0;
  currentSession.breaksTaken = 0;
  currentSession.totalReadings = 0;
  currentSession.scoreSum = 0;
  
  lastBreakTime = millis();
  
  // Setup Web Server
  setupWebServer();
  
  // Create FreeRTOS Tasks
  xTaskCreatePinnedToCore(
    postureDetectionTask,
    "PostureDetection",
    STACK_SIZE_POSTURE,
    NULL,
    2,
    &postureTaskHandle,
    0  // Core 0
  );
  
  Serial.println("\n========================================");
  Serial.println("System Ready!");
  Serial.println("========================================");
  Serial.printf("Connect to WiFi: %s\n", AP_SSID);
  Serial.printf("Password: %s\n", AP_PASSWORD);
  Serial.printf("Dashboard: http://%s\n", WiFi.softAPIP().toString().c_str());
  Serial.printf("Live Stream: http://%s/stream\n", WiFi.softAPIP().toString().c_str());
  Serial.println("========================================\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Check break mode timeout
  if(breakMode && (millis() - breakStartTime > BREAK_DURATION)) {
    breakMode = false;
    Serial.println("Break ended");
  }
  
  // Monitor WiFi AP
  int clients = WiFi.softAPgetStationNum();
  static int lastClients = 0;
  if(clients != lastClients) {
    Serial.printf("Connected clients: %d\n", clients);
    lastClients = clients;
  }
  
  delay(1000);
}