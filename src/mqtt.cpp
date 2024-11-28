#include <PubSubClient.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include <constants.h>
#include <ArduinoJson.h>

unsigned long lastMsg = 0;
char* appletData;
boolean newapplet = false;
boolean deserilize = false;
unsigned char * appletdecoded;
unsigned char * base64decoded;
char hostName[11];
char status_topic[18];
char current_app[19];
char applet_topic[18];
char brightness_topic[22];
char heap_topic[20];

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
    Serial.printf("Message received [%s]\n", topic);
    // uint32_t freeHeap = ESP.getFreeHeap();
    // Serial.printf("Heap available: %d\n", freeHeap);
    if (strcmp(topic, applet_topic) == 0) {
        payload[length] = '\0';
        appletData = (char*)payload;
        DeserializationError error = deserializeJson(doc, appletData, length);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        const char* applet_name;
        const char* applet;
        if (doc.containsKey("applet")) 
            applet_name = doc["applet"];
        if (doc.containsKey("payload"))
            applet = doc["payload"];

        free(base64decoded);
        base64decoded = base64_decode((const unsigned char*)applet, strlen(applet), &outputLength);
        if (base64decoded && outputLength > 1 && strncmp((const char*)base64decoded, "GIF8", 4) == 0) {
            free(appletdecoded);
            appletdecoded = (unsigned char *)malloc(outputLength);
            memcpy(appletdecoded, base64decoded, outputLength);

            newapplet = true;
            currentMode = APPLET;
            client.publish(current_app, applet_name, true);
        } else {
            Serial.println(F("Error decoding base64. Not valid base64 or not GIF image"));
        }
    }
    if (strcmp(topic, brightness_topic) == 0) {
        payload[length] = '\0';
        brightness = atoi((char*)payload);
    }
}


void mqttReconnect(char* mqtt_user, char* mqtt_password) {
    Serial.println(F("mqttReconnect"));

    while (!client.connected()) {
        Serial.println(F("Trying to reconnect to MQTT"));
        Serial.println(hostName);

    if (client.connect(hostName))
        { //, mqtt_user, mqtt_password)) {
            Serial.println(F("Connected to MQTT"));
            client.publish(status_topic, (const char *)"reconnected");
            client.subscribe(status_topic);
            client.subscribe(brightness_topic);
            client.subscribe(applet_topic);
        }
        else
        {
            Serial.print(F("Failed, rc="));
            Serial.print(client.state());
            Serial.println(F("Retrying in 5 seconds"));
            delay(5000);
        }
    }
}