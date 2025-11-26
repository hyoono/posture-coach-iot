/*
 * WEMOS D1 R1 - Posture Coach IoT Firmware
 * Complete production-ready implementation with all sensors and HTTP communication
 * 
 * CRITICAL: Pin configuration based on WEMOS D1 R1 board silkscreen
 * TX (D1) and RX (D0) are RESERVED for Serial Monitor - DO NOT USE
 * D5 (GPIO14) has hardware pull-down - AVOIDED for buttons
 * 
 * Hardware Connections (Based on Board Silkscreen):
 * - Serial Monitor: RX<-D0 (RESERVED), TX->D1 (RESERVED)
 * - HC-SR04 Ultrasonic: Trigger=D2, Echo=D13 (header side SCK)
 * - OLED Display I2C: SDA=D4 (D14/SDA), SCL=D3 (D15/SCL)
 * - Buttons (Internal Pull-up): Break=D11, Snooze=D6, Privacy=D7
 * - RGB LED (Common Cathode): Red=D5, Green=D8, Blue=D9
 * - Piezo Buzzer: A0 (digital output)
 * - WiFi Status: Built-in LED (LED_BUILTIN)
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

// Pin Definitions - WEMOS D1 R1 Specific (TX/RX kept free for Serial)
// NOTE: D5 (GPIO14) has hardware pull-down issue - avoid for buttons
#define PIN_RX D0           // RESERVED for Serial RX - DO NOT USE
#define PIN_TX D1           // RESERVED for Serial TX - DO NOT USE
#define TRIG_PIN D2         // HC-SR04 Trigger
#define OLED_SCL D3         // I2C Clock (D15/SCL on silkscreen)
#define OLED_SDA D4         // I2C Data (D14/SDA on silkscreen)
#define LED_RED D5          // RGB LED Red (220Ω resistor) - MOVED FROM D8
#define BUTTON2_PIN D6      // Snooze (with internal pull-up)
#define BUTTON3_PIN D7      // Privacy (with internal pull-up)
#define LED_GREEN D8        // RGB LED Green (220Ω resistor) - MOVED FROM D9
#define LED_BLUE D9         // RGB LED Blue (220Ω resistor) - MOVED FROM D10
#define BUTTON1_PIN D11     // Manual Break (with internal pull-up) - MOVED FROM D5
#define ECHO_PIN D13        // HC-SR04 Echo (header side SCK)
#define BUZZER_PIN A0       // Buzzer (used as digital output)
#define STATUS_LED LED_BUILTIN  // WiFi status indicator

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// WiFi Configuration - Must match ESP32-CAM AP settings
const char* ssid = "PostureCoach-AP";
const char* password = "posture123";  // Match ESP32-CAM AP password
const char* serverIP = "192.168.4.1";
const int serverPort = 80;

// Timing Constants
#define DISTANCE_READ_INTERVAL 2000    // Read distance every 2 seconds
#define POLL_INTERVAL 3000             // Poll ESP32-CAM every 3 seconds
#define BUTTON_DEBOUNCE 50             // Button debounce delay (ms)
#define OLED_UPDATE_INTERVAL 1000      // Update OLED every 1 second
#define WIFI_RECONNECT_INTERVAL 5000   // Try reconnecting every 5 seconds

// Global Objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient wifiClient;

// State Variables
unsigned long lastPollTime = 0;
unsigned long lastDistanceRead = 0;
unsigned long lastOLEDUpdate = 0;
unsigned long lastWiFiAttempt = 0;
unsigned long lastButton1Press = 0;
unsigned long lastButton2Press = 0;
unsigned long lastButton3Press = 0;

bool privacyMode = false;
bool breakMode = false;
int postureScore = 0;
float distance = 0;
String currentStatus = "Good";
String ledColor = "GREEN";

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n========================================");
  Serial.println("WEMOS D1 R1 Posture Coach Starting...");
  Serial.println("========================================");
  Serial.println("Note: TX/RX pins free for Serial Monitor");
  Serial.println("========================================\n");
  
  setupPins();
  setupOLED();
  setupWiFi();
  
  // Initial feedback
  setRGBColor(0, 255, 0);  // Green = ready
  playBuzzerTone(1000, 100);
  delay(100);
  playBuzzerTone(1500, 100);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Ready!");
  display.println("Waiting for");
  display.println("ESP32-CAM...");
  display.display();
  
  Serial.println("Setup complete - ready to communicate\n");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // WiFi connection management
  if (WiFi.status() != WL_CONNECTED) {
    if (currentMillis - lastWiFiAttempt >= WIFI_RECONNECT_INTERVAL) {
      lastWiFiAttempt = currentMillis;
      reconnectWiFi();
    }
    digitalWrite(STATUS_LED, (currentMillis / 500) % 2);  // Blink status LED
    delay(100);
    return;
  } else {
    digitalWrite(STATUS_LED, LOW);  // Solid on when connected
  }
  
  // Check buttons (every loop with debouncing)
  checkButtons();
  
  // Read distance sensor periodically
  if (currentMillis - lastDistanceRead >= DISTANCE_READ_INTERVAL) {
    lastDistanceRead = currentMillis;
    
    if (!privacyMode && !breakMode) {
      distance = readDistance();
      if (distance > 0) {
        sendSensorData();
      }
    }
  }
  
  // Poll ESP32-CAM for posture data and commands
  if (currentMillis - lastPollTime >= POLL_INTERVAL) {
    lastPollTime = currentMillis;
    
    if (!privacyMode && !breakMode) {
      pollPostureData();
      pollCommands();
    }
  }
  
  // Update OLED display periodically
  if (currentMillis - lastOLEDUpdate >= OLED_UPDATE_INTERVAL) {
    lastOLEDUpdate = currentMillis;
    updateOLED();
  }
  
  delay(10);  // Small delay to prevent watchdog issues
}

void setupPins() {
  Serial.println("Initializing pins...");
  
  // Buttons with internal pull-up resistors
  pinMode(BUTTON1_PIN, INPUT_PULLUP);  // D5 - Manual Break
  pinMode(BUTTON2_PIN, INPUT_PULLUP);  // D6 - Snooze
  pinMode(BUTTON3_PIN, INPUT_PULLUP);  // D7 - Privacy
  
  // RGB LED pins (Common Cathode - HIGH = ON)
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  
  // Buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  // WiFi status LED
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, HIGH);
  
  Serial.println("Pins initialized");
  Serial.println("  Buttons: D11 (Break), D6 (Snooze), D7 (Privacy) - internal pull-up");
  Serial.println("  RGB LED: D5 (Red), D8 (Green), D9 (Blue)");
  Serial.println("  Ultrasonic: D2 (Trig), D13 (Echo)");
  Serial.println("  Buzzer: A0");
  Serial.println("  I2C: D4 (SDA), D3 (SCL)");
  
  // Read initial button states for debugging
  delay(100);
  Serial.println("\nInitial button states (should be HIGH with pull-ups):");
  Serial.print("  Button 1 (D11): ");
  Serial.println(digitalRead(BUTTON1_PIN) == HIGH ? "HIGH (released)" : "LOW (STUCK OR PRESSED!)");
  Serial.print("  Button 2 (D6): ");
  Serial.println(digitalRead(BUTTON2_PIN) == HIGH ? "HIGH (released)" : "LOW (pressed)");
  Serial.print("  Button 3 (D7): ");
  Serial.println(digitalRead(BUTTON3_PIN) == HIGH ? "HIGH (released)" : "LOW (pressed)");
  Serial.println();
}

void setupOLED() {
  Serial.println("Initializing OLED display...");
  
  // Initialize I2C with custom pins: SDA=D4, SCL=D3
  Wire.begin(OLED_SDA, OLED_SCL);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("ERROR: OLED initialization failed!");
    Serial.println("Check I2C connections: SDA=D4, SCL=D3");
    for(;;) {
      digitalWrite(LED_RED, HIGH);
      delay(500);
      digitalWrite(LED_RED, LOW);
      delay(500);
    }
  }
  
  Serial.println("OLED initialized successfully");
  Serial.println("  I2C Address: 0x3C");
  Serial.println("  Resolution: 128x64");
  
  // Display startup message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Posture Coach");
  display.println("IoT System");
  display.println("---------------");
  display.println("Initializing...");
  display.println("Please wait...");
  display.display();
}

void setupWiFi() {
  Serial.println("\nConnecting to WiFi Access Point...");
  Serial.print("  SSID: ");
  Serial.println(ssid);
  Serial.print("  Server IP: ");
  Serial.println(serverIP);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connecting to");
  display.println(ssid);
  display.display();
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    if (attempts % 5 == 0) {
      display.print(".");
      display.display();
    }
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected successfully!");
    Serial.print("  IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Connected");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(2000);
  } else {
    Serial.println("WiFi connection failed!");
    Serial.println("  Check ESP32-CAM is powered on");
    Serial.println("  Verify AP is broadcasting");
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Failed!");
    display.println("Check ESP32-CAM");
    display.display();
  }
}

void reconnectWiFi() {
  Serial.println("WiFi disconnected - attempting reconnection...");
  setRGBColor(255, 255, 0);  // Yellow during reconnect
  
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi reconnected!");
    Serial.print("  IP: ");
    Serial.println(WiFi.localIP());
    setRGBColor(0, 255, 0);  // Green = connected
  } else {
    Serial.println("Reconnection failed, will retry...");
    setRGBColor(255, 0, 0);  // Red = failed
  }
}

float readDistance() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Read echo pulse (timeout after 30ms = ~5m max distance)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  if (duration == 0) {
    Serial.println("Distance: Timeout (no echo)");
    return -1;  // Timeout
  }
  
  // Calculate distance in cm (speed of sound = 343 m/s = 0.0343 cm/µs)
  float dist = duration * 0.034 / 2.0;
  
  // Validate reading (HC-SR04 range: 2cm to 400cm)
  if (dist < 2 || dist > 400) {
    Serial.printf("Distance: Out of range (%.1f cm)\n", dist);
    return -1;
  }
  
  Serial.printf("Distance measured: %.1f cm\n", dist);
  return dist;
}

void checkButtons() {
  // Read button states
  int btn1State = digitalRead(BUTTON1_PIN);
  int btn2State = digitalRead(BUTTON2_PIN);
  int btn3State = digitalRead(BUTTON3_PIN);
  
  // Button 1: Manual Break (Active LOW with internal pull-up)
  // Only trigger if button has been released first (prevents stuck button)
  static bool btn1WasHigh = true;
  if (btn1State == HIGH) {
    btn1WasHigh = true;
  }
  if (btn1State == LOW && btn1WasHigh && 
      (millis() - lastButton1Press > BUTTON_DEBOUNCE)) {
    lastButton1Press = millis();
    btn1WasHigh = false;
    handleBreakButton();
  }
  
  // Button 2: Snooze Alerts (Active LOW with internal pull-up)
  static bool btn2WasHigh = true;
  if (btn2State == HIGH) {
    btn2WasHigh = true;
  }
  if (btn2State == LOW && btn2WasHigh && 
      (millis() - lastButton2Press > BUTTON_DEBOUNCE)) {
    lastButton2Press = millis();
    btn2WasHigh = false;
    handleSnoozeButton();
  }
  
  // Button 3: Privacy Mode Toggle (Active LOW with internal pull-up)
  static bool btn3WasHigh = true;
  if (btn3State == HIGH) {
    btn3WasHigh = true;
  }
  if (btn3State == LOW && btn3WasHigh && 
      (millis() - lastButton3Press > BUTTON_DEBOUNCE)) {
    lastButton3Press = millis();
    btn3WasHigh = false;
    handlePrivacyButton();
  }
}

void handleBreakButton() {
  Serial.println("\n[BUTTON] Manual Break requested");
  playBuzzerTone(800, 200);
  delay(250);
  playBuzzerTone(800, 200);
  
  breakMode = true;
  currentStatus = "Break Mode";
  
  // Send button press to ESP32-CAM
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(serverIP) + "/api/input/button";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<128> doc;
  doc["button"] = 1;
  doc["action"] = "PRESSED";
  doc["timestamp"] = millis();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  Serial.printf("Break request sent, response code: %d\n", httpCode);
  
  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("Response: " + response);
  }
  
  http.end();
}

void handleSnoozeButton() {
  Serial.println("\n[BUTTON] Snooze requested");
  playBuzzerTone(1200, 150);
  
  currentStatus = "Snoozed 5min";
  
  // Send button press to ESP32-CAM
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(serverIP) + "/api/input/button";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<128> doc;
  doc["button"] = 2;
  doc["action"] = "PRESSED";
  doc["timestamp"] = millis();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  Serial.printf("Snooze request sent, response code: %d\n", httpCode);
  
  http.end();
}

void handlePrivacyButton() {
  privacyMode = !privacyMode;
  
  Serial.print("\n[BUTTON] Privacy Mode: ");
  Serial.println(privacyMode ? "ENABLED" : "DISABLED");
  
  if (privacyMode) {
    playBuzzerTone(2000, 100);
    delay(120);
    playBuzzerTone(2000, 100);
    delay(120);
    playBuzzerTone(2000, 100);
    setRGBColor(255, 0, 255);  // Magenta for privacy
  } else {
    playBuzzerTone(1000, 100);
    delay(150);
    playBuzzerTone(1500, 100);
    setRGBColor(0, 255, 0);  // Green for active
  }
  
  currentStatus = privacyMode ? "Privacy ON" : "Monitoring";
  
  // Send button press to ESP32-CAM
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(serverIP) + "/api/input/button";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  
  StaticJsonDocument<128> doc;
  doc["button"] = 3;
  doc["action"] = "PRESSED";
  doc["timestamp"] = millis();
  
  String payload;
  serializeJson(doc, payload);
  
  int httpCode = http.POST(payload);
  Serial.printf("Privacy toggle sent, response code: %d\n", httpCode);
  
  http.end();
}

void setRGBColor(int r, int g, int b) {
  // Common Cathode RGB LED: HIGH = ON, LOW = OFF
  // Values 0-255 for PWM control
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}

void playBuzzerTone(int frequency, int duration) {
  // Play tone on buzzer (A0 pin used as digital output)
  tone(BUZZER_PIN, frequency, duration);
}

void playBuzzerPattern(String pattern) {
  if (pattern == "SUCCESS") {
    // Ascending pleasant tones
    playBuzzerTone(800, 100);
    delay(120);
    playBuzzerTone(1000, 100);
    delay(120);
    playBuzzerTone(1200, 100);
  } else if (pattern == "WARNING") {
    // 2 medium beeps
    playBuzzerTone(1000, 200);
    delay(250);
    playBuzzerTone(1000, 200);
  } else if (pattern == "URGENT") {
    // 3 loud ascending beeps
    playBuzzerTone(1200, 150);
    delay(180);
    playBuzzerTone(1400, 150);
    delay(180);
    playBuzzerTone(1600, 150);
  } else if (pattern == "DISTANCE") {
    // Rapid short beeps
    for (int i = 0; i < 5; i++) {
      playBuzzerTone(1800, 80);
      delay(100);
    }
  } else if (pattern == "BREAK_REMINDER") {
    // Gentle two-tone
    playBuzzerTone(800, 250);
    delay(300);
    playBuzzerTone(600, 250);
  }
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  // Header
  display.println("Posture Coach IoT");
  display.println("================");
  
  // Posture Score
  display.print("Score: ");
  display.println(postureScore);
  
  // Distance
  display.print("Dist: ");
  if (distance > 0) {
    display.print(distance, 1);
    display.println(" cm");
  } else {
    display.println("-- cm");
  }
  
  // Status
  display.println("----------------");
  display.println(currentStatus);
  
  // WiFi indicator
  if (WiFi.status() == WL_CONNECTED) {
    display.print("WiFi: ");
    display.print(WiFi.RSSI());
    display.println(" dBm");
  } else {
    display.println("WiFi: Disconnected");
  }
  
  display.display();
}

void sendSensorData() {
  if (distance < 0 || distance > 400) {
    return;  // Don't send invalid readings
  }
  
  Serial.printf("\n[HTTP POST] Sending distance: %.1f cm\n", distance);
  
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(serverIP) + "/api/sensor/distance";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);
  
  // Create JSON payload
  StaticJsonDocument<256> doc;
  doc["distance"] = distance;
  doc["unit"] = "cm";
  doc["timestamp"] = millis();
  
  String payload;
  serializeJson(doc, payload);
  
  Serial.println("Payload: " + payload);
  
  int httpCode = http.POST(payload);
  
  if (httpCode > 0) {
    Serial.printf("Response code: %d\n", httpCode);
    if (httpCode == 200) {
      String response = http.getString();
      Serial.println("Response: " + response);
    }
  } else {
    Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void pollPostureData() {
  Serial.println("\n[HTTP GET] Polling posture data from ESP32-CAM...");
  
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(serverIP) + "/api/posture/current";
  http.begin(client, url);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("Posture data received: " + response);
    
    // Parse JSON response
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      postureScore = doc["score"] | 0;
      currentStatus = doc["status"] | "unknown";
      
      Serial.printf("  Score: %d\n", postureScore);
      Serial.printf("  Status: %s\n", currentStatus.c_str());
      
      // Update LED based on score
      if (postureScore >= 90) {
        setRGBColor(0, 255, 0);  // Green - Excellent
        ledColor = "GREEN";
      } else if (postureScore >= 75) {
        setRGBColor(0, 0, 255);  // Blue - Good
        ledColor = "BLUE";
      } else if (postureScore >= 60) {
        setRGBColor(255, 255, 0);  // Yellow - Fair
        ledColor = "YELLOW";
      } else {
        setRGBColor(255, 0, 0);  // Red - Poor
        ledColor = "RED";
      }
    } else {
      Serial.print("JSON parse error: ");
      Serial.println(error.c_str());
    }
  } else if (httpCode > 0) {
    Serial.printf("GET failed with code: %d\n", httpCode);
  } else {
    Serial.printf("GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void pollCommands() {
  Serial.println("\n[HTTP GET] Polling commands from ESP32-CAM...");
  
  HTTPClient http;
  WiFiClient client;
  
  String url = "http://" + String(serverIP) + "/api/commands/pending";
  http.begin(client, url);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("Command received: " + response);
    
    // Parse JSON response
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      String command = doc["command"] | "NONE";
      
      if (command != "NONE") {
        String value = doc["value"] | "";
        String pattern = doc["pattern"] | "";
        int duration = doc["duration"] | 0;
        
        Serial.printf("  Command: %s\n", command.c_str());
        Serial.printf("  Value: %s\n", value.c_str());
        Serial.printf("  Pattern: %s\n", pattern.c_str());
        
        // Execute command
        if (command == "LED") {
          executeLEDCommand(value, pattern);
        } else if (command == "BUZZER") {
          executeBuzzerCommand(pattern, duration);
        } else if (command == "BREAK") {
          executeBreakCommand(value, duration);
        } else if (command == "OLED") {
          currentStatus = value;
        }
      } else {
        Serial.println("  No pending commands");
      }
    }
  } else if (httpCode > 0) {
    Serial.printf("GET failed with code: %d\n", httpCode);
  } else {
    Serial.printf("GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  
  http.end();
}

void executeLEDCommand(String color, String pattern) {
  Serial.printf("[EXECUTE] LED: %s (%s)\n", color.c_str(), pattern.c_str());
  
  if (color == "RED") {
    setRGBColor(255, 0, 0);
  } else if (color == "GREEN") {
    setRGBColor(0, 255, 0);
  } else if (color == "BLUE") {
    setRGBColor(0, 0, 255);
  } else if (color == "YELLOW") {
    setRGBColor(255, 255, 0);
  } else if (color == "OFF") {
    setRGBColor(0, 0, 0);
  }
  
  ledColor = color;
}

void executeBuzzerCommand(String pattern, int duration) {
  Serial.printf("[EXECUTE] Buzzer: %s (%d ms)\n", pattern.c_str(), duration);
  playBuzzerPattern(pattern);
}

void executeBreakCommand(String action, int duration) {
  Serial.printf("[EXECUTE] Break: %s (%d sec)\n", action.c_str(), duration);
  
  if (action == "START") {
    breakMode = true;
    currentStatus = "Break Time!";
    setRGBColor(0, 0, 255);  // Blue for break
    playBuzzerPattern("BREAK_REMINDER");
  } else if (action == "END") {
    breakMode = false;
    currentStatus = "Monitoring";
    setRGBColor(0, 255, 0);  // Green resume
  }
}