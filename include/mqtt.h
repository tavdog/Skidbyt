#ifndef MQTT_H
#define MQTT_H
#define MSG_BUFFER_SIZE	(255)

extern unsigned long lastMsg;
extern char msg[MSG_BUFFER_SIZE];
extern int value;
extern char* subscribersInScreen;
extern char* currentSubscribers;
extern char* appletData;
extern unsigned char * appletdecoded;
extern unsigned char * base64decoded;
extern size_t outputLength;
extern int currentMode;
extern boolean newapplet;
extern int brightness;

extern boolean deserilize;

extern WiFiClient wifiClient;
extern PubSubClient client;

void mqttCallback(char* topic, byte* payload, unsigned int length);
void mqttReconnect(char* mqtt_user, char* mqtt_password);


#endif
