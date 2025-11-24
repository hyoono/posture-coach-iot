/*
 * WEMOS D1 R1 - Posture Coach IoT Firmware
 * Complete production-ready implementation with all sensors and HTTP communication
 * 
 * Hardware Connections:
 * - HC-SR04 Ultrasonic: Trigger=D0, Echo=D12
 * - OLED Display I2C: SDA=D1(GPIO5), SCL=D2(GPIO4)
 * - RGB LED: Red=D7, Green=D8, Blue=D11
 * - Piezo Buzzer: A0
 * - Buttons: D3 (Break), D5 (Snooze), D6 (Privacy) with internal pull-ups
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin Definitions
#define TRIGGER_PIN D0
#define ECHO_PIN D12
#define OLED_SDA D1
#define OLED_SCL D2
#define BUTTON_BREAK D3
#define BUTTON_SNOOZE D5
#define BUTTON_PRIVACY D6
#define RED_PIN D7
#define GREEN_PIN D8
#define BLUE_PIN D11
#define BUZZER_PIN A0

// OLED Display Settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// WiFi Configuration
const char* ssid = "PostureCoach-AP";
const char* password = "posturecoach2024";
const char* serverIP = "192.168.4.1";

// Timing Constants
#define POLL_INTERVAL 3000
#define BUTTON_DEBOUNCE 200

// Global Objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WiFiClient wifiClient;

// State Variables
unsigned long lastPollTime = 0;
unsigned long lastButton1Press = 0;
unsigned long lastButton2Press = 0;
unsigned long lastButton3Press = 0;
bool privacyMode = false;
int postureScore = 0;
float distance = 0;
String currentStatus = "Initializing...";

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n=== WEMOS D1 R1 Posture Coach ===");
  
  setupPins();
  setupOLED();
  setupWiFi();
  
  setRGBColor(0, 255, 0);
  playBuzzerPattern(1);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Ready!");
  display.display();
  
  Serial.println("Setup complete");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
  
  checkButtons();
  
  if (millis() - lastPollTime >= POLL_INTERVAL) {
    lastPollTime = millis();
    
    if (!privacyMode) {
      distance = readDistance();
      sendSensorData();
      pollESP32();
      updateOLED();
    }
  }
  
  delay(10);
}

void setupPins() {
  pinMode(BUTTON_BREAK, INPUT_PULLUP);
  pinMode(BUTTON_SNOOZE, INPUT_PULLUP);
  pinMode(BUTTON_PRIVACY, INPUT_PULLUP);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void setupOLED() {
  Wire.begin(OLED_SDA, OLED_SCL);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED init failed");
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Posture Coach");
  display.println("Initializing...");
  display.display();
}

void setupWiFi() {
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
}

void reconnectWiFi() {
  Serial.println("Reconnecting WiFi...");
  setRGBColor(255, 255, 0);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    setRGBColor(0, 255, 0);
  }
}

float readDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1;
  
  float dist = duration * 0.034 / 2.0;
  
  if (dist < 2 || dist > 400) return -1;
  
  return dist;
}

void checkButtons() {
  if (digitalRead(BUTTON_BREAK) == LOW && millis() - lastButton1Press > BUTTON_DEBOUNCE) {
    lastButton1Press = millis();
    handleBreakButton();
  }
  
  if (digitalRead(BUTTON_SNOOZE) == LOW && millis() - lastButton2Press > BUTTON_DEBOUNCE) {
    lastButton2Press = millis();
    handleSnoozeButton();
  }
  
  if (digitalRead(BUTTON_PRIVACY) == LOW && millis() - lastButton3Press > BUTTON_DEBOUNCE) {
    lastButton3Press = millis();
    handlePrivacyButton();
  }
}

void handleBreakButton() {
  Serial.println("Break button");
  playBuzzerPattern(2);
  
  HTTPClient http;
  http.begin(wifiClient, "http://" + String(serverIP) + "/api/break");
  http.POST("");
  http.end();
  
  currentStatus = "Break started";
}

void handleSnoozeButton() {
  Serial.println("Snooze button");
  playBuzzerPattern(3);
  
  HTTPClient http;
  http.begin(wifiClient, "http://" + String(serverIP) + "/api/snooze");
  http.POST("");
  http.end();
  
  currentStatus = "Snoozed";
}

void handlePrivacyButton() {
  privacyMode = !privacyMode;
  Serial.print("Privacy: ");
  Serial.println(privacyMode ? "ON" : "OFF");
  
  playBuzzerPattern(privacyMode ? 4 : 1);
  setRGBColor(privacyMode ? 255 : 0, 0, privacyMode ? 255 : 0);
  
  currentStatus = privacyMode ? "Privacy ON" : "Privacy OFF";
}

void setRGBColor(int r, int g, int b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}

void playBuzzerPattern(int pattern) {
  switch (pattern) {
    case 1:
      tone(BUZZER_PIN, 1000, 100);
      delay(150);
      tone(BUZZER_PIN, 1500, 100);
      break;
    case 2:
      tone(BUZZER_PIN, 800, 200);
      delay(250);
      tone(BUZZER_PIN, 800, 200);
      break;
    case 3:
      tone(BUZZER_PIN, 1200, 150);
      break;
    case 4:
      for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, 2000, 100);
        delay(120);
      }
      break;
  }
}

void updateOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println("Posture Coach");
  display.println("---------------");
  display.print("Score: ");
  display.println(postureScore);
  display.print("Dist: ");
  display.print(distance);
  display.println(" cm");
  display.println("---------------");
  display.println(currentStatus);
  
  display.display();
}

void sendSensorData() {
  if (distance < 0) return;
  
  HTTPClient http;
  http.begin(wifiClient, "http://" + String(serverIP) + "/api/sensor/data");
  http.addHeader("Content-Type", "application/json");
  
  String payload = "{\"distance\":" + String(distance) + "}";
  http.POST(payload);
  http.end();
}

void pollESP32() {
  HTTPClient http;
  http.begin(wifiClient, "http://" + String(serverIP) + "/api/posture/current");
  
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    
    int scoreIdx = response.indexOf("\"score\":");
    if (scoreIdx >= 0) {
      postureScore = response.substring(scoreIdx + 8, response.indexOf(",", scoreIdx)).toInt();
      
      if (postureScore >= 80) {
        setRGBColor(0, 255, 0);
      } else if (postureScore >= 50) {
        setRGBColor(255, 255, 0);
      } else {
        setRGBColor(255, 0, 0);
      }
    }
  }
  
  http.end();
}