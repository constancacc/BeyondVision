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
#include <esp_now.h>
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
#define BTN_Pin 16
//----------------------------------------
uint8_t broadcastAddress[] = { 0x24, 0x6F, 0x28, 0xF0, 0x66, 0x64 };  //--> REPLACE WITH THE MAC Address of your receiver.

//----------------------------------------Variables to accommodate the data to be sent.
int BTN_State = 0;
bool isBuzzing = false;
const int crashThreshold = 50; //fall detection
String success;

// Must match with ESP32 paired.
typedef struct struct_message {
  bool isBuzzing;
  bool crashDetected;
} struct_message;

struct_message send_Data;  // Create a struct_message to send data.


// const char ssid[] = "eduroam";
const char ssid[] = "Vodafone-2C12CC";
const char pass[] = "dZhZf2Bh8mjJqXKD";

WiFiClient net;
MQTTClient client = MQTTClient(256);

const char MQTT_BROKER_ADRESS[] = "broker.hivemq.com";
const int MQTT_PORT = 1883;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
  Serial.println(">>>>>");
}


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

  WiFi.mode(WIFI_STA);  //--> Set device as a Wi-Fi Station

  //----------------------------------------Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //----------------------------------------

  //----------------------------------------Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  //----------------------------------------

  //----------------------------------------Register peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));  // Clear the peer info structure
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  //----------------------------------------

  //----------------------------------------Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  //----------------------------------------

  Serial.println("Setup completed");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void loop() {
  ESP.getFreeHeap();


  // Set values to send 
  BTN_State = digitalRead(BTN_Pin);  //--> Reads and holds button states.
  
  /* Get a new sensor event */ 
  sensors_event_t event; 
  accel.getEvent(&event);
 
  /* Display the results (acceleration is measured in m/s^2) 
  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");*/

  if (!client.connected()) {
    Serial.println("Reconnecting to MQTT broker...");
    connectToMQTT();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Reconnected to WiFi!");
  }

  client.loop();

  bool crashDetected = (abs(event.acceleration.x) > crashThreshold || abs(event.acceleration.y) > crashThreshold || abs(event.acceleration.z) > crashThreshold);
  if (crashDetected) {
    Serial.println("Crash detected!");
    send_Data.crashDetected = true;

    //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&send_Data, sizeof(send_Data));
    client.publish("bracelet", "crash");
    Serial.println("Published: bracelet/crash");

    /*if (result == ESP_OK) {
      Serial.println("Crash notification sent with success");
    } else {
      Serial.println("Error sending crash notification");
    }*/

    delay(2000);
  } else {
    send_Data.crashDetected = false;
  }

  
  if (BTN_State == LOW) {
    isBuzzing = !isBuzzing;

    Serial.println(isBuzzing);
    send_Data.isBuzzing = isBuzzing;

    Serial.println();
    Serial.print(">>>>> ");
    Serial.println("Send data");

    //----------------------------------------Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&send_Data, sizeof(send_Data));

    if (result == ESP_OK) {
      Serial.println("Sent with success");
    } else {
      Serial.println("Error sending the data");
    }
    //----------------------------------------

    while(BTN_State == LOW) {
      BTN_State = digitalRead(BTN_Pin);
      delay(10);
    }

    delay(200);  // Debounce delay
  }
}


void connectToMQTT() {
  client.begin(MQTT_BROKER_ADRESS, MQTT_PORT, net);
  Serial.println("Connecting to broker...");

  while(!client.connect("braceletESP32BeyondVision")) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to broker!");
}