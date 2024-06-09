//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Two-Way communication ESP32
/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-two-way-communication-esp32/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
//Pulseira
//----------------------------------------Load libraries
#include <WiFi.h>
#include <Wire.h>
#include <MQTTClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define EAP_ANONYMOUS_IDENTITY "anonymous@uc.pt"
#define EAP_IDENTITY "2023242951@student.uc.pt" 
#define EAP_PASSWORD "2023242951" 
#define EAP_USERNAME "2023242951@student.uc.pt"

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
//----------------------------------------
#define BTN_Pin 15
//----------------------------------------
int BTN_State = 0;
bool isBuzzing = false;
const int crashThreshold = 50; //fall detection
String success;

const char ssid[] = "Vodafone-2C12CC";
const char pass[] = "dZhZf2Bh8mjJqXKD";

WiFiClient net;
MQTTClient client = MQTTClient(256);

const char MQTT_BROKER_ADRESS[] = "broker.hivemq.com";
const int MQTT_PORT = 1883;

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;  // Interval between reconnection attempts

void setup() {
  Serial.begin(115200);
  Serial.println("Accelerometer Test");
  WiFi.begin(ssid, pass);
  // WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
  Serial.println("Connecting to wifi ...");

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.println("Connected to WiFi!");

  connectToMQTT();
  
  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }

  accel.setRange(ADXL345_RANGE_16_G);

  pinMode(BTN_Pin, INPUT_PULLUP);

  Serial.println("Setup completed");
}

void loop() {
  ESP.getFreeHeap();

  // Set values to send 
  BTN_State = digitalRead(BTN_Pin);  // Reads and holds button states.

  // Get a new sensor event 
  sensors_event_t event; 
  accel.getEvent(&event);

  // Handle WiFi reconnection
  if (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      Serial.println("Reconnecting to WiFi...");
      WiFi.begin(ssid, pass);
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Reconnected to WiFi!");
      }
    }
  }

  // Handle MQTT reconnection
  if (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    connectToMQTT();
  } else {
    client.loop();
  }

  // Detect crash
  bool crashDetected = (abs(event.acceleration.x) > crashThreshold || abs(event.acceleration.y) > crashThreshold || abs(event.acceleration.z) > crashThreshold);
  if (crashDetected) {
    Serial.println("Crash detected!");

    client.publish("bracelet", "crash");
    Serial.println("Published: bracelet/crash");

    delay(2000);
  }

  // Handle button press
  if (BTN_State == LOW) {
    isBuzzing = !isBuzzing;

    Serial.println(isBuzzing);

    if (isBuzzing) {
      client.publish("findMyCane", "buzzing");
    } else {
      client.publish("findMyCane", "not_buzzing");
    }

    Serial.println("Published: findMyCane");

    // Debounce the button
    while (BTN_State == LOW) {
      BTN_State = digitalRead(BTN_Pin);
      delay(10);
    }

    delay(200);  // Additional debounce delay
  }
}

bool connectToMQTT() {
  client.begin(MQTT_BROKER_ADRESS, MQTT_PORT, net);
  Serial.println("Connecting to broker...");

  if (client.connect("braceletESP32BeyondVision")) {
    Serial.println("Connected to MQTT broker!");
    return true;
  }
  Serial.print("Failed to connect to MQTT broker, rc=");
  return false;
}
