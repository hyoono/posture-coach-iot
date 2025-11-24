// Configuration header file for WEMOS D1 R1

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// Pin Definitions
#define HC_SR04_TRIGGER_PIN 2
#define HC_SR04_ECHO_PIN 3
#define OLED_SDA_PIN 4
#define OLED_SCL_PIN 5
#define RGB_LED_PIN 6
#define BUZZER_PIN 7
#define BUTTON_PIN 8

// Server Configuration
#define SERVER_IP "192.168.4.1"

// Polling Intervals
#define SENSOR_POLL_INTERVAL_MS 1000
#define BUTTON_POLL_INTERVAL_MS 50

// HTTP Timeout Values
#define HTTP_TIMEOUT_MS 5000

// Sensor Calibration Constants
#define HC_SR04_CALIBRATION_FACTOR 0.0343

// Button Debounce Delays
#define BUTTON_DEBOUNCE_DELAY_MS 200

// OLED Display Settings
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Buzzer Frequency Patterns
#define BUZZER_FREQ_HIGH 2000
#define BUZZER_FREQ_LOW 1000

// LED Color Codes
#define LED_COLOR_RED 0xFF0000
#define LED_COLOR_GREEN 0x00FF00
#define LED_COLOR_BLUE 0x0000FF

// Debug Settings
#define DEBUG_ENABLED 1

#endif // CONFIG_H