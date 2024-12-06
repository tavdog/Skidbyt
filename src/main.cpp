#include <FS.h>
#include "SPIFFS.h"

#include <Arduino.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> 
#include <time.h>
#include <ArduinoOTA.h>

#include <constants.h>
#include <mqtt.h>
//#include <password.h>

#include <Tidbyt_ESP32_RGB64x32MatrixPanel-I2S-DMA.h>

#include <SmartMatrix.h>

#include <GifDecoder.h>


#define DISPLAY_TIME_SECONDS 10
#define NUMBER_FULL_CYCLES   100

#define USE_SMARTMATRIX         1
#define ENABLE_SCROLLING        1


void setupOTA();
void setupTopics();

#include "smart_matrix_setup.h"

// Variables de configuración para el AP WiFi
// char host[] = "PLM";
// char separator[] = "_";
// char SSID[10];
byte mac[6];
char macFull[6];

void configModeCallback (WiFiManager *myWiFiManager) {
    
    scrollingLayer.stop();
    scrollingLayer.setColor(WHITE);
    scrollingLayer.setMode(wrapForward);
    scrollingLayer.setSpeed(80);
    scrollingLayer.setFont(font6x10);
    scrollingLayer.start("Unable to connect. Access Point:", -1);

    // Show SSID
    backgroundLayer.setFont(font5x7);
    backgroundLayer.drawString(0, 21, LIGHT_BLUE, hostName);
    backgroundLayer.swapBuffers();
}

StaticJsonDocument<3500> document;

char mqtt_server[40];
char mqtt_port[8];
char mqtt_user[40];
char mqtt_password[40];

bool saveConfig = false;

void saveConfigCallback () {
    saveConfig = true;
    Serial.println(F("We need to save the config"));
}

void setup() {
    decoder.setScreenClearCallback(screenClearCallback);
    decoder.setUpdateScreenCallback(updateScreenCallback);
    decoder.setDrawPixelCallback(drawPixelCallback);

    Serial.begin(115200);
    delay(1000);

    Serial.println(F("Mounting FS..."));

    if (SPIFFS.begin(true)) {
        if (SPIFFS.exists("/config.json")) {
            Serial.println(F("FS mounted"));
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile) {
                size_t size = configFile.size();
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument doc(256);
                DeserializationError error = deserializeJson(doc, buf.get());
                if (error) {
                    Serial.println(F("There was an error reading the config.json file"));
                    return;
                }
                JsonObject json = doc.as<JsonObject>();

                serializeJson(json, Serial);
                if (!json.isNull()) {

                    strcpy(mqtt_server, json["mqtt_server"]);
                    strcpy(mqtt_port, json["mqtt_port"]);
                    strcpy(mqtt_user, json["mqtt_user"]);
                    strcpy(mqtt_password, json["mqtt_password"]);

                } else {
                    Serial.println(F("Something happened"));
                }
            }
        }
    } else {
        // Serial.println("Couldn't mount the FS");
    } 

    WiFiManagerParameter custom_mqtt_server("server", "MQTT server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "MQTT port", mqtt_port, 8);
    WiFiManagerParameter custom_mqtt_user("user", "MQTT user", mqtt_user, 40);
    WiFiManagerParameter custom_mqtt_pass("pass", "MQTT password", mqtt_password, 40);

    WiFiManager wifiManager;
    WiFi.macAddress(mac);
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(macFull, "%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
    snprintf(hostName, 11, PSTR("PLM-%s"),macFull);

    matrix.addLayer(&backgroundLayer); 
    matrix.addLayer(&scrollingLayer); 
    // matrix.addLayer(&indexedLayer);
    matrix.begin(28000);
    matrix.setBrightness(defaultBrightness);
    scrollingLayer.setOffsetFromTop(defaultScrollOffset);
    backgroundLayer.enableColorCorrection(true);


    scrollingLayer.setColor(WHITE);
    scrollingLayer.setMode(wrapForward);
    scrollingLayer.setSpeed(60);
    scrollingLayer.setFont(font6x10);
    scrollingLayer.start("Connecting to WiFi...", -1);

    Serial.println(F("Connecting to WiFi..."));
    WiFi.setHostname(hostName);
    WiFi.setAutoReconnect(true);

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setTimeout(180);
    wifiManager.setCleanConnect(true);
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_user);
    wifiManager.addParameter(&custom_mqtt_pass);

    if (!wifiManager.autoConnect(hostName, "password")) {
        delay(3000);
    }
    scrollingLayer.stop();
    scrollingLayer.setMode(stopped);
    scrollingLayer.start(hostName, -1);
    delay(3000);

    Serial.print(F("Hostname: "));
    Serial.println(hostName);

    setupOTA();
    setupTopics();

    client.setServer(mqtt_server, atoi(mqtt_port));
    client.connect(hostName, mqtt_user, mqtt_password);
    client.setBufferSize(30000);
    client.setCallback(mqttCallback);

    if (client.connected()) {
        Serial.println(F("Connected to MQTT (main loop)"));
        client.publish(status_topic, (const char *)"up");
        client.subscribe(status_topic);
        client.subscribe(applet_topic);
        client.subscribe(brightness_topic);

    }

    if (saveConfig) {
        strcpy(mqtt_server, custom_mqtt_server.getValue());
        strcpy(mqtt_port, custom_mqtt_port.getValue());
        strcpy(mqtt_user, custom_mqtt_user.getValue());
        strcpy(mqtt_password, custom_mqtt_pass.getValue());

        DynamicJsonDocument doc(256);
        JsonObject json = doc.to<JsonObject>();

        json["mqtt_server"] = mqtt_server;
        json["mqtt_port"]   = mqtt_port;
        json["mqtt_user"]   = mqtt_user;
        json["mqtt_password"]   = mqtt_password;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
        Serial.println(F("There was an error opening the file to write"));
        }

        serializeJson(json, configFile);
        configFile.close();
        saveConfig = false;
    }

}

void loop() {
    // Try to reconnect to wifi if connection lost
    while (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0,0,0,0)) {
        WiFi.reconnect();
        Serial.println(F("Trying to reconnect to WiFi"));
        delay(10000);
    }

    client.loop();
    ArduinoOTA.handle();

    if (!client.connected()) {
        Serial.println(F("Not connected to MQTT. Trying to reconnect."));
        mqttReconnect(mqtt_user, mqtt_password);
    }

    if (brightness > 0) {
        matrix.setBrightness((brightness*255) / 100);
        brightness = -1;
    }
    if (currentMode == WELCOME) {
        scrollingLayer.setColor(WHITE);
        scrollingLayer.setMode(wrapForward);
        scrollingLayer.setSpeed(60);
        scrollingLayer.setFont(font6x10);
        scrollingLayer.start("Waiting for applets...", -1);
        currentMode = NONE;
    }
    if (currentMode == APPLET) {
        scrollingLayer.stop();
        static uint32_t lastFrameDisplayTime = 0;
        static unsigned int currentFrameDelay = 0;

        unsigned long now = millis();

        if((millis() - lastFrameDisplayTime) > currentFrameDelay) {
            if (newapplet) {
                decoder.startDecoding((uint8_t *)appletdecoded, outputLength);
                newapplet = false;
            }
            decoder.decodeFrame(false);

            lastFrameDisplayTime = now;
            currentFrameDelay = decoder.getFrameDelay_ms();
        }
    }
}

void setupTopics() {
    snprintf_P(status_topic, 18, PSTR("%s/%s/status"), TOPIC_PREFIX, macFull);
    snprintf_P(current_app, 19, PSTR("%s/%s/current"), TOPIC_PREFIX, macFull);
    snprintf_P(applet_topic, 18, PSTR("%s/%s/applet"), TOPIC_PREFIX, macFull);
    Serial.println(applet_topic);
    snprintf_P(brightness_topic, 22, PSTR("%s/%s/brightness"), TOPIC_PREFIX, macFull);
    snprintf_P(heap_topic, 20, PSTR("%s/%s/freeheap"), TOPIC_PREFIX, macFull);
}

void setupOTA() {

    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname(hostName);

    // No authentication by default
    // ArduinoOTA.setPassword(otaPassword);

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart([]() {
        scrollingLayer.stop();
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        // Serial.println("Start updating " + type);
        })
        .onEnd([]() {
            // Serial.println("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            // Serial.printf("Error[%u]: ", error);
            // if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        //     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        //     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        //     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        //     else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

    ArduinoOTA.begin();
}