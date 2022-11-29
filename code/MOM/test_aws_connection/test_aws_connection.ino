#include "secrets.h"
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "ESP32Ping.h"

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

void pingTest() {
  bool ret = Ping.ping("a3368omdfuonoa-ats.iot.us-east-1.amazonaws.com", 10);
  float avg_time_ms = Ping.averageTime();
  Serial.print("Ping test finished. Average time ");
  Serial.print(avg_time_ms);
  Serial.println(" milliseconds.");
}

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("\nConnecting to AWS IOT ");

  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(10);
  }

  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  Serial.println("\nAWS IoT Connected!");
}

void publishMessage() {
  StaticJsonDocument<200> doc;
  doc["time"] = millis();
  doc["sender_mac"] = WiFi.macAddress();
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);  // print to client

  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  //  StaticJsonDocument<200> doc;
  //  deserializeJson(doc, payload);
  //  const char* message = doc["message"];
}

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  delay(1000);
  connectAWS();
}

void loop() {
  digitalWrite(13, HIGH);
  client.loop();
  publishMessage();
  publishMessage();
  publishMessage();
  Serial.println("Messages published!");
  while (true) {
    pingTest();
    delay(1000);
  };
}