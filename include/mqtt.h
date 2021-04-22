#ifndef MQTT_H
#define MQTT_H

extern char* appletData;
extern unsigned char * appletdecoded;
extern unsigned char * base64decoded;
extern size_t outputLength;
extern int currentMode;
extern boolean newapplet;
extern int brightness;

extern char hostName[11];
extern char status_topic[18];
extern char current_app[19];
extern char applet_topic[18];
extern char brightness_topic[22];
extern char heap_topic[20];

extern boolean deserilize;

extern WiFiClient wifiClient;
extern PubSubClient client;

void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttReconnect(char* mqtt_user, char* mqtt_password);


#endif
