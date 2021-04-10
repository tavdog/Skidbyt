/*
  Copyright (C) 2020  Domótica Fácil con Jota en YouTube

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
