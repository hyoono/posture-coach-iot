// esp32-cam/config.h

#ifndef CONFIG_H
#define CONFIG_H

// WiFi AP credentials
#define WIFI_SSID "Your_SSID"
#define WIFI_PASSWORD "Your_Password"
#define WIFI_IP "192.168.1.100"
#define WIFI_GATEWAY "192.168.1.1"
#define WIFI_SUBNET "255.255.255.0"

// Camera pin definitions
#define CAMERA_PIN_CLOCK 14
#define CAMERA_PIN_DATA0 13
#define CAMERA_PIN_DATA1 12
#define CAMERA_PIN_DATA2 15
#define CAMERA_PIN_DATA3 2
#define CAMERA_PIN_DATA4 4
#define CAMERA_PIN_DATA5 5
#define CAMERA_PIN_DATA6 18
#define CAMERA_PIN_DATA7 19

// Posture detection thresholds and constants
#define POSTURE_THRESHOLD 100
#define ANOMALY_THRESHOLD 200

// Break interval timing constants
#define BREAK_INTERVAL_MIN 5 // in minutes
#define BREAK_INTERVAL_MAX 30 // in minutes

// FreeRTOS task priorities and stack sizes
#define TASK_PRIORITY_HIGH 5
#define TASK_PRIORITY_MEDIUM 3
#define TASK_PRIORITY_LOW 1
#define TASK_STACK_SIZE 2048

// API endpoint paths
#define API_URL "http://your.api.endpoint"
#define API_POSTURE_DETECT_PATH "/posture/detect"
#define API_SENSOR_DATA_PATH "/sensor/data"

// Sensor value ranges
#define TEMP_MIN 15 // Minimum temperature
#define TEMP_MAX 30 // Maximum temperature
#define HUMIDITY_MIN 20 // Minimum humidity
#define HUMIDITY_MAX 80 // Maximum humidity

// Debug flags
#define DEBUG_MODE 1
#define DEBUG_SERIAL_PORT 115200

// Feature enable/disable flags
#define FEATURE_POSTURE_DETECTION_ENABLED 1
#define FEATURE_SENSOR_MONITORING_ENABLED 1

#endif // CONFIG_H
