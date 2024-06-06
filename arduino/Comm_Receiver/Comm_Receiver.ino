//Bengala
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0xF0, 0x0E, 0x44}; // MAC Address of ESP32 A

#define echoPinLong 32  // Echo Pin
#define trigPinLong 33  // Trigger Pin
#define echoPin 14  // Echo Pin
#define trigPin 27  // Trigger Pin
#define ledPin 2  // Onboard LED
#define buttonOnOff 25 // Button pin
#define buzzerPin 26

int sensorValue;
int maximumRange = 20;   // Maximum range needed
int minimumRange = 0;    // Minimum range needed
int duration, distance;  // Duration used to calculate distance
int durationLong, distanceLong; 
int buttonOnOffState = 0;
bool isCaneOn = false;
bool isBuzzing = false;

typedef struct struct_message {
    bool currentBuzzingState;
} struct_message_send;

struct_message receive_Data; // Create a struct_message to receive data.

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_Data, incomingData, sizeof(receive_Data));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);
  isBuzzing = receive_Data.currentBuzzingState;
  Serial.print("Receive Data: ");
  Serial.println(isBuzzing);
  Serial.println("<<<<<");
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

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  // Clear the peer info structure
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv); //--> Register for a callback function that will be called when data is received
}

void loop() {

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
