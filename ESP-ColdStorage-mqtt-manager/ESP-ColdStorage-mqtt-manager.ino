
#include <Arduino.h>
//#include <CMMC_Manager.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <MqttConnector.h>

//#define BUTTON_INPUT_PIN 2
//CMMC_Manager manager(BUTTON_INPUT_PIN, LED_BUILTIN);

WiFiClient client;
String api_key = "SGUH5EDVHXIALPHO";
uint32_t pevmillis = 0;

#include "init_mqtt.h"
#include "_publish.h"
#include "_receive.h"

/* WIFI INFO */
#ifndef WIFI_SSID
#define WIFI_SSID        "ESPERT-3020"
#define WIFI_PASSWORD    "espertap"
#endif

String MQTT_HOST        = "mqtt.cmmc.io";
String MQTT_USERNAME    = "";
String MQTT_PASSWORD    = "";
String MQTT_CLIENT_ID   = "";
String MQTT_PREFIX      = "CMMC";
int    MQTT_PORT        = 1883;

int PUBLISH_EVERY       = 1000;

MqttConnector *mqtt;

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(LED_BUILTIN, OUTPUT);
  //  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);

}

void init_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID, WIFI_PASSWORD);
    delay(300);
  }
  Serial.println("WiFi Connected.");
  digitalWrite(LED_BUILTIN, HIGH);
  //  manager.start();
}

void doHttpGet(float t, float h) {
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");

  // http://api.thingspeak.com/update?api_key=5T4WXGZFE1PZPS2K&field1=0
  http.begin("http://api.thingspeak.com/update?api_key=" + api_key + "&field1=" + String(t) + "&field2=" + String(h)); //HTTP

  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    Serial.print("[CONTENT]\n");

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void setup()
{
  init_hardware();
  init_wifi();
  init_mqtt();
}

void loop()
{
  uint32_t conMillis = millis();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (conMillis - pevmillis >= 15000)  {
    digitalWrite(LED_BUILTIN, LOW);
    doHttpGet(t, h);
    delay(2000);
    digitalWrite(LED_BUILTIN, HIGH);
    pevmillis = conMillis;
  }
  delay(200);
  mqtt->loop();
}
