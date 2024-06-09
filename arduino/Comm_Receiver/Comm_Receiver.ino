//Bengala
#include <WiFi.h>
#include <MQTTClient.h>

#define echoPinLong 32  // Echo Pin
#define trigPinLong 33  // Trigger Pin
#define echoPin 14  // Echo Pin
#define trigPin 27  // Trigger Pin
#define ledPin 2  // Onboard LED
#define buttonOnOff 25 // Button pin
#define buzzerPin 26

const char ssid[] = "Vodafone-2C12CC";
const char pass[] = "dZhZf2Bh8mjJqXKD";
const char MQTT_BROKER_ADRESS[] = "broker.hivemq.com";
const int MQTT_PORT = 1883;

WiFiClient net;
MQTTClient client = MQTTClient(256);

int sensorValue;
int maximumRange = 20;   // Maximum range needed
int minimumRange = 0;    // Minimum range needed
int duration, distance;  // Duration used to calculate distance
int durationLong, distanceLong; 
int buttonOnOffState = 0;
bool isCaneOn = false;
bool isBuzzing = false;

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;  // Interval between reconnection attempts

void messageReceived(String &topic, String &payload) {
  Serial.println("Incoming: " + topic + " - " + payload);
  if (topic == "findMyCane") {
    isBuzzing = (payload == "buzzing");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPinLong, OUTPUT);
  pinMode(echoPinLong, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonOnOff, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  delay(2000);

  WiFi.begin(ssid, pass);
  Serial.println("Connecting to wifi ...");

  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.println("Connected to WiFi!");

  client.begin(MQTT_BROKER_ADRESS, MQTT_PORT, net);
  client.onMessage(messageReceived);

  connectToMQTT();

  Serial.println("Setup completed");
}

void loop() {
  ESP.getFreeHeap();

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

  if (isBuzzing) {
    tone(buzzerPin, 300);
    delay(1000); // Sound the buzzer for 500ms
  }

  buttonOnOffState = digitalRead(buttonOnOff);

  // Toggle alarm on/off when the button is pressed
  if (buttonOnOffState == LOW) {
    isCaneOn = !isCaneOn;
    digitalWrite(ledPin, isCaneOn ? HIGH : LOW);  // Turn on LED when alarm is enabled
    if (!isCaneOn) {
      noTone(buzzerPin); // Turn off buzzer when alarm is turned off
    }
    delay(200); // Debounce delay
  }

  if (isCaneOn) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 30000);

    // Measure distance for the second sensor
    delay(50); // Small delay
    digitalWrite(trigPinLong, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPinLong, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPinLong, LOW);
    durationLong = pulseIn(echoPinLong, HIGH, 30000);

    //Calculate the distance (in cm) based on the speed of sound.
    distance = duration / 58.2;
    distanceLong = durationLong / 58.2;

    Serial.print("Distance 1: ");
    Serial.print(distance);
    Serial.print(" cm, Distance 2 (long): ");
    Serial.print(distanceLong);
    Serial.println(" cm");

    // obstacle is detected within 50cm
    if (distance < maximumRange || distanceLong < maximumRange) {
      tone(buzzerPin, 1000);
      delay(500); // Sound the buzzer for 500ms
    } else {
      noTone(buzzerPin); // Turn off the buzzer if no obstacle
    }
  } else {
    // Turn off the buzzer if alarm is off
    noTone(buzzerPin);
  }
  delay(200);
}

bool connectToMQTT() {
  Serial.println("Connecting to broker...");
  if (client.connect("caneESP32BeyondVision")) {
    Serial.println("Connected to MQTT broker!");
    client.subscribe("findMyCane");
    return true;
  }
  Serial.print("Failed to connect to MQTT broker, rc=");
  return false;
}
