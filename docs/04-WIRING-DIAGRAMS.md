# Wiring Diagrams Documentation

## Complete WEMOS D1 R1 Pin Mapping Table

| Pin Number | Pin Name        | Function                |
|------------|----------------|-------------------------|
| 1          | D0             | GPIO0                   |
| 2          | D1             | GPIO5 (TXD)             |
| 3          | D2             | GPIO4 (SDA)             |
| 4          | D3             | GPIO0 (SSID)            |
| 5          | D4             | GPIO2 (SCL)             |
| 6          | D5             | GPIO14 (D5)             |
| 7          | D6             | GPIO12 (D6)             |
| 8          | D7             | GPIO13 (D7)             |
| 9          | D8             | GPIO15 (D8)             |
| 10         | GND            | Ground                  |
| 11         | 3V3            | Power                   |

## ASCII Art Circuit Diagrams

```plaintext
+-----------+    +--------+      +----+
| HC-SR04  |    | OLED   |      | RGB|
|  VCC    --+    | VCC    +------+ V | 
| GND     --+----| GND    |       | LED|
| TRIG    --+    | SDA    +-------|   |
| ECHO    --+    | SCL    |       |   |
+-----------+    +--------+      +----+
```

## Connection Instructions for Each Component

### HC-SR04 Ultrasonic Sensor Wiring
- Connect VCC to 3.3V.
- Connect GND to GND.
- Connect TRIG to D2 (GPIO4).
- Connect ECHO to D3 (GPIO0).

### OLED I2C Display Wiring
- Connect VCC to 3.3V.
- Connect GND to GND.
- Connect SDA to D1 (GPIO5).
- Connect SCL to D2 (GPIO4).

### RGB LED with Resistors Wiring
- Connect the Red pin through a resistor to D5 (GPIO14).
- Connect the Green pin through a resistor to D6 (GPIO12).
- Connect the Blue pin through a resistor to D7 (GPIO13).
- Connect GND to GND.

### Buzzer Connection
- Connect VCC to 3.3V.
- Connect GND to GND.
- Connect the signal pin to D4 (GPIO2).

### Button Connections with Internal Pull-Ups
- Connect one terminal to D0 (GPIO0).
- Connect the other terminal to GND.
- Enable internal pull-up resistor in software.

## Power Distribution Diagram
```plaintext
+-------+
| 3.3V  |
+-------+
   |   
+---+---+      +---------+
| GND   | -->  | GND     |
|       |       |         |
|   ... |       |  ...    |
+-------+       +---------+
```

## Breadboard Layout Suggestions
- Use a standard breadboard layout with power rails on top and bottom.
- Group components based on functionality (e.g. sensors, displays).

## Wire Color Coding Recommendations
- Red for power (VCC).
- Black for ground (GND).
- Yellow for signal wires.
- Other colors for specific connections (e.g. green for I2C).

## Common Wiring Mistakes to Avoid
- Mixing up VCC and GND connections.
- Not using resistors for LEDs.
- Forgetting to enable pull-ups for buttons.

## Testing Procedures for Each Connection
- Verify each connection visually before powering devices.
- Use a multimeter to check for continuity in important connections.
- Test each component individually before integrating into the system.
