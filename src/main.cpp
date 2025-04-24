#include <Arduino.h>
#include <BleMouse.h>

// Mouse settings
#define MOUSE_UPDATE_INTERVAL 70 // ms; time between mouse updates; lower = more responsive but more resource-intensive
#define MOUSE_DEAD_BAND 10 // raw value; minimum movement to register a mouse move
#define MOUSE_SENSITIVITY 0.006 // 0.0 - 1.0; higher value = more pixels per degree of joystick movement
#define MOUSE_EXPONENT_THRESHOLD 950 // filtered raw value; threshold above which to apply exponential scaling; 0 = linear
#define MOUSE_EXPONENT 1.25 // scale factor for mouse movement; 1.0 = linear, >1 = exponential
#define MOUSE_MAX_SPEED 2000 // max speed in pixels per `MOUSE_UPDATE_INTERVAL`

// Battery settings
#define BATT_CHECK_INTERVAL 60000 // 1 minute
#define R1 100000.0 // resistor divider R1 value in ohms
#define R2 100000.0 // resistor divider R2 value in ohms

// Pin definitions
#define POT_X_PIN 0 // analog pin for X-axis potentiometer
#define POT_Y_PIN 1 // analog pin for Y-axis potentiometer
#define BTN_PIN 2 // digital pin for button
#define BATTERY_PIN 3 // analog pin for battery voltage measurement

// BLE Mouse settings
#define BLE_CHECK_INTERVAL 1000 // ms; time between BLE connection checks
#define BLE_NAME "Glove80 Fin Mouse" // BLE device name
#define BLE_MANUFACTURER "DIY" // BLE manufacturer name

// Variable definitions
BleMouse bleMouse(BLE_NAME, BLE_MANUFACTURER, 100);
int x_zero, y_zero;
int x, y, last_x, last_y;
int button_state, last_button_state;
int x_move, y_move;

// Function prototypes
void readFilterSave();
void readButton();
void sendMouseCommands();
void updateBatteryLevel();
float getBatteryVoltage();
uint8_t batteryPercentage(float v);
void printState();

void setup() {
  // Initialize pins
  pinMode(POT_X_PIN, INPUT);
  pinMode(POT_Y_PIN, INPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

  // Initialize serial
  Serial.begin(9600);
  delay(300);

  // Initialize stick position
  Serial.print("Stick calibrating...");
  x_zero = analogRead(POT_X_PIN);
  y_zero = analogRead(POT_Y_PIN);

  // Initialize BLE Mouse
  Serial.println("Starting BLE Mouse...");
  bleMouse.begin();
}

void loop() {
  static unsigned long lastMouseUpdate = 0;
  static unsigned long lastBatteryUpdate = 0;
  static unsigned long lastConnectionCheck = 0;
  static bool isConnected = false;
  unsigned long currentMillis = millis();

  // Prioritize mouse updates, then check other things
  if (isConnected && (currentMillis - lastMouseUpdate >= MOUSE_UPDATE_INTERVAL)) {
    // Update the mouse state
    lastMouseUpdate = currentMillis;
    readButton();
    readFilterSave();
    sendMouseCommands();
    printState();
  } else {
    // Check if the mouse is connected
    if (currentMillis - lastConnectionCheck >= BLE_CHECK_INTERVAL) {
      lastConnectionCheck = currentMillis;
      bool latestConnection = bleMouse.isConnected();
      if (isConnected != latestConnection) {
        isConnected = latestConnection;
        if (isConnected) {
          Serial.print("BLE mouse connected at ");
          Serial.println(millis());
        } else {
          Serial.print("BLE mouse disconnected at ");
          Serial.println(millis());
        }
      }
    }

    // Check battery status
    if (currentMillis - lastBatteryUpdate >= BATT_CHECK_INTERVAL) {
      lastBatteryUpdate = currentMillis;
      updateBatteryLevel();
    }
  }
}

// Read and filter the analog values from the potentiometers
void readFilterSave() {
  int raw_x = analogRead(POT_X_PIN) - x_zero;
  int raw_y = analogRead(POT_Y_PIN) - y_zero;

  // Invert axes
  raw_y = -raw_y;
  raw_x = -raw_x;

  // Calculate the magnitude of the joystick vector
  float magnitude = sqrt(raw_x * raw_x + raw_y * raw_y);

  // Apply the dead band to the magnitude
  if (magnitude < MOUSE_DEAD_BAND) {
    raw_x = 0;
    raw_y = 0;
  } else {
    // Scale the magnitude to start at zero after the dead band
    float scale = (magnitude - MOUSE_DEAD_BAND) / magnitude;
    raw_x *= scale;
    raw_y *= scale;
  }

  // Save the filtered values
  last_x = x;
  last_y = y;
  x = raw_x;
  y = raw_y;
}

// Read the button state
void readButton() {
  last_button_state = button_state;
  button_state = !digitalRead(BTN_PIN);
}

// Send mouse commands based on the button state and joystick position
void sendMouseCommands() {
  // Send button clicks
  if (last_button_state != button_state) {
    if (button_state)
      bleMouse.press(MOUSE_LEFT);
    else
      bleMouse.release(MOUSE_LEFT);
  }

  // Calculate the magnitude of the joystick vector
  float magnitude = sqrt(x * x + y * y);

  // Apply exponential scaling to the magnitude
  float scaled_magnitude = (magnitude < MOUSE_EXPONENT_THRESHOLD)
    ? magnitude
    : pow(magnitude - MOUSE_EXPONENT_THRESHOLD, MOUSE_EXPONENT) + MOUSE_EXPONENT_THRESHOLD;

  // Normalize the x and y components and scale them by the scaled magnitude
  float scale = (magnitude > 0) ? scaled_magnitude / magnitude : 0;
  float scaled_x = x * scale;
  float scaled_y = y * scale;

  // Scale the joystick values
  x_move = MOUSE_SENSITIVITY * scaled_x;
  y_move = MOUSE_SENSITIVITY * scaled_y;

  // Limit the speed
  if (x_move > MOUSE_MAX_SPEED) {
    x_move = MOUSE_MAX_SPEED;
  } else if (x_move < -MOUSE_MAX_SPEED) {
    x_move = -MOUSE_MAX_SPEED;
  }
  if (y_move > MOUSE_MAX_SPEED) {
    y_move = MOUSE_MAX_SPEED;
  } else if (y_move < -MOUSE_MAX_SPEED) {
    y_move = -MOUSE_MAX_SPEED;
  }

  // Send the move command
  if (x_move != 0 || y_move != 0) {
    bleMouse.move(x_move, y_move);
  }
}

// Check the battery voltage and update the battery level
void updateBatteryLevel() {
  float voltage = getBatteryVoltage();
  uint8_t batteryLevel = batteryPercentage(voltage);
  bleMouse.setBatteryLevel(batteryLevel);
  Serial.print("Battery voltage: ");
  Serial.print(voltage);
  Serial.print("V, Battery level: ");
  Serial.print(batteryLevel);
  Serial.println("%");
}

// Get the battery voltage
float getBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = (raw / 4095.0) * 3.3; // Assuming 12-bit ADC and 3.3V reference
  float batteryVoltage = voltage * ((R1 + R2) / R2);
  return batteryVoltage;
}

// Convert the battery voltage to a percentage
uint8_t batteryPercentage(float v) {
  if (v >= 4.2) return 100;
  if (v <= 3.2) return 0;
  return (uint8_t)((v - 3.2) * 100.0 / (4.2 - 3.2)); // Linear approx
}

// Print the current state of the joystick and button
void printState() {
  Serial.printf("x:%5d  |  y:%5d  |  x_move:%5d  |  y_move:%5d  |  button:%1d\n", x, y, x_move, y_move, button_state);
}