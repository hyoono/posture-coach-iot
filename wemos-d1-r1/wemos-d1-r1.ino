# WEMOS D1 R1 Arduino Sketch

// Include libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi credentials
const char* ssid = "ESP32-CAM";
const char* password = "your_password";

// Create server object
ESP8266WebServer server(80);

// Function to handle root requests
void handleRoot() {
  server.send(200, "text/plain", "Hello from WEMOS D1 R1!");
}

// Setup function
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi!");
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

// Loop function
void loop() {
  server.handleClient();
  // Here, you would read sensors and control feedback devices
}