#include "HX711.h"

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ESP32Servo.h>

// === HX711 Setup ===
#define LOADCELL_DOUT_PIN 35
#define LOADCELL_SCK_PIN 27
HX711 scale;
float calibration_factor = 825.0;

// === Pin Definitions ===
#define IN1 17
#define IN2 18
#define IN3 19
#define IN4 21
#define LED_PIN 14
#define SERVO_PIN 13

// === MQTT Settings ===
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* controlTopic = "/StepperControl";
const char* weightTopic = "/PetFeeder/weight";

WiFiClient espClient;
PubSubClient client(espClient);
Servo cleaningServo;

// Stepper Motor Half-Step Sequence
const int steps[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// === MQTT Callback ===
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);

  if (message == "fill") {
    Serial.println("Simulating fill: LED ON for 5 seconds...");
    digitalWrite(LED_PIN, HIGH);
    delay(5000);
    digitalWrite(LED_PIN, LOW);
    delay(5000);
  }
  else if (message == "clean") {
    Serial.println("Cleaning: Servo to 0°...");
    cleaningServo.write(0);
    delay(5000);
    cleaningServo.write(180);
    Serial.println("Cleaning done: Servo back to 180°.");
  }
  else {
    int input = message.toInt();
    if (input != 0) {
      float rounds = float(input) / 400.0;
      int totalSteps = rounds * 2048;
      int direction = input > 0 ? 1 : 0;
      rotateStepper(totalSteps, direction);
    }
  }
}

// === Stepper Motor Control ===
void rotateStepper(int stepsCount, int direction) {
  for (int i = 0; i < stepsCount; i++) {
    int idx = direction ? i % 8 : 7 - (i % 8);
    digitalWrite(IN1, steps[idx][0]);
    digitalWrite(IN2, steps[idx][1]);
    digitalWrite(IN3, steps[idx][2]);
    digitalWrite(IN4, steps[idx][3]);
    delayMicroseconds(1000);
  }
}

// === Reconnect to MQTT ===
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(controlTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  delay(1000);

  // Pin setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Servo setup
  cleaningServo.setPeriodHertz(50);
  cleaningServo.attach(SERVO_PIN, 500, 2400);
  cleaningServo.write(180);

  // WiFiManager setup
  WiFiManager wm;
  String apName = "ESP32_Setup";
  if (!wm.autoConnect(apName.c_str())) {
    Serial.println("Failed to connect. Restarting...");
    ESP.restart();
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // MQTT setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // HX711 setup
  Serial.println("HX711 Calibration Demo for ESP32");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("Remove all weight from scale...");
  delay(2000);
  scale.tare();
  Serial.println("Scale tared.");
  Serial.println("Place a known weight on the scale...");
  delay(5000);
  long reading = scale.get_units(10);
  Serial.print("Raw reading: ");
  Serial.println(reading);
  Serial.println("Adjust the calibration_factor variable until the output matches your known weight.");
  Serial.print("Current calibration factor: ");
  Serial.println(calibration_factor);
  scale.set_scale(calibration_factor);
}

// === Main Loop ===
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (scale.is_ready()) {
    float weight = scale.get_units(5);
    Serial.print("Weight: ");
    Serial.print(weight, 2);
    Serial.println(" grams");

    // Publish to MQTT
    String payload = String(weight, 2);
    client.publish(weightTopic, payload.c_str());
  } else {
    Serial.println("HX711 not found.");
  }

  delay(1000);
}
