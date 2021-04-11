#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <constants.h>

unsigned long lastMsg = 0;
int value = 0;
char* appletData;
boolean newapplet = false;
boolean deserilize = false;
unsigned char * appletdecoded;
size_t outputLength;
extern "C" {
    #include "crypto/base64.h"
}

// MODOS
int currentMode = WELCOME;
int brightness = -1;

// Llamada de vuelta sobre MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message received [%s]\n", topic);

  if (strcmp(topic, APPLET_TOPIC) == 0) {
    payload[length] = '\0';
    appletData = (char*)payload;
    appletdecoded = base64_decode((const unsigned char*)appletData, strlen(appletData), &outputLength);
    if (appletdecoded && outputLength > 1 && strncmp((const char*)appletdecoded, "GIF8", 4) == 0) {
      newapplet = true;
      currentMode = APPLET;
      // Serial.println("Mode changed to applet");
    } else {
      Serial.println("Error decoding base64. Not valid base64 or not GIF image");
    }
  }
  if (strcmp(topic, BRIGHTNESS_TOPIC) == 0) {
    payload[length] = '\0';
    brightness = atoi((char*)payload);
  }
}

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void mqttReconnect(char* mqtt_user, char* mqtt_password) {
  Serial.println("mqttReconnect");

  while (!client.connected()) {
    Serial.println("Trying to reconnect to MQTT");
    if (client.connect(MQTT_CLIENT, mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT");
      client.publish(STATUS_TOPIC, (const char *)"up");
      client.subscribe(STATUS_TOPIC);
      client.subscribe(BRIGHTNESS_TOPIC);
      client.subscribe(APPLET_TOPIC);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 5 seconds");
      delay(5000);
    }
  }
}