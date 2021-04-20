#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <constants.h>
#include <ArduinoJson.h>

unsigned long lastMsg = 0;
int value = 0;
char* appletData;
boolean newapplet = false;
boolean deserilize = false;
unsigned char * appletdecoded;
unsigned char * base64decoded;
size_t outputLength;
extern "C" {
    #include "crypto/base64.h"
}

int currentMode = WELCOME;
int brightness = -1;

StaticJsonDocument<64> doc;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Serial.printf("Message received [%s]\n", topic);
  // Serial.printf("Heap available: %d\n", ESP.getFreeHeap());

  if (strcmp(topic, APPLET_TOPIC) == 0) {
    payload[length] = '\0';
    appletData = (char*)payload;
    DeserializationError error = deserializeJson(doc, appletData, length);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    const char* applet_name = doc["applet"];
    const char* applet = doc["payload"];

    free(base64decoded);
    base64decoded = base64_decode((const unsigned char*)applet, strlen(applet), &outputLength);
    if (base64decoded && outputLength > 1 && strncmp((const char*)base64decoded, "GIF8", 4) == 0) {
      free(appletdecoded);
      appletdecoded = (unsigned char *)malloc(outputLength);
      memcpy(appletdecoded, base64decoded, outputLength);

      newapplet = true;
      currentMode = APPLET;
      client.publish(CURRENT_APP, applet_name, true);
    } else {
      Serial.println(F("Error decoding base64. Not valid base64 or not GIF image"));
    }
  }
  if (strcmp(topic, BRIGHTNESS_TOPIC) == 0) {
    payload[length] = '\0';
    brightness = atoi((char*)payload);
  }
}


void mqttReconnect(char* mqtt_user, char* mqtt_password) {
  Serial.println(F("mqttReconnect"));

  while (!client.connected()) {
    Serial.println(F("Trying to reconnect to MQTT"));
    if (client.connect(MQTT_CLIENT, mqtt_user, mqtt_password)) {
      Serial.println(F("Connected to MQTT"));
      client.publish(STATUS_TOPIC, (const char *)"up");
      client.subscribe(STATUS_TOPIC);
      client.subscribe(BRIGHTNESS_TOPIC);
      client.subscribe(APPLET_TOPIC);
    } else {
      Serial.print(F("Failed, rc="));
      Serial.print(client.state());
      Serial.println(F("Retrying in 5 seconds"));
      delay(5000);
    }
  }
}