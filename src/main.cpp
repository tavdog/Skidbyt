#include <FS.h>
#include "SPIFFS.h"

#include <Arduino.h>
#include <LCBUrl.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <constants.h>
#include <Tidbyt_ESP32_RGB64x32MatrixPanel-I2S-DMA.h>
#include <SmartMatrix.h>
#include <GifDecoder.h>

// lots of smart matrix setup happens in here
#include "smart_matrix_setup.h"

// AP WiFi
char host[] = "Skidbyt";
char separator[] = "_";
char SSID[10];
byte mac[6];
char macFull[6];



uint8_t *downloadGIF(char* curl, size_t &fileSize)
{
    LCBUrl url;
    url.setUrl(curl); 
    if (WiFi.status() == WL_CONNECTED)
    {
        // Function to download the GIF file
            WiFiClientSecure client;
            client.setInsecure(); // Disable SSL certificate verification (use only if your server does not have a valid certificate)

            Serial.println("Connecting to server...");
            if (!client.connect(url.getHost().c_str(), url.getPort()))
            {
                Serial.println("Connection to server failed!");
                return nullptr;
            }

            // Construct the HTTP GET request
            String request = String("GET ") + url.getPath() + " HTTP/1.1\r\n" +
                             "Host: " + url.getHost() + "\r\n" +
                             "Connection: close\r\n\r\n";

            // Send the request
            client.print(request);

            // Wait for the response
            while (client.connected() && !client.available())
            {
                delay(100);
            }

            // Parse HTTP response
            if (!client.find("Content-Length:"))
            {
                Serial.println("Failed to find Content-Length header!");
                return nullptr;
            }

            fileSize = client.parseInt(); // Get the file size (we already did a find "Content-Length" above)
            Serial.printf("Content-Length: %d bytes\n", fileSize);

            // Skip HTTP headers
            if (!client.find("\r\n\r\n"))
            {
                Serial.println("Invalid HTTP headers!");
                return nullptr;
            }

            // Allocate memory for the GIF file
            uint8_t *gifBuffer = (uint8_t *)malloc(fileSize);
            if (!gifBuffer)
            {
                Serial.println("Failed to allocate memory for GIF!");
                return nullptr;
            }

            // Read the GIF file into the buffer
            size_t bytesRead = 0;
            while (bytesRead < fileSize)
            {
                size_t chunkSize = client.readBytes(gifBuffer + bytesRead, fileSize - bytesRead);
                if (chunkSize == 0)
                {
                    Serial.println("Failed to read data!");
                    free(gifBuffer);
                    return nullptr;
                }
                bytesRead += chunkSize;
            }

            Serial.println("GIF file downloaded successfully!");
            return gifBuffer;
    }
    else
    {
        Serial.println("WiFi disconnected");
        delay(2000);
    }
}

void configModeCallback(WiFiManager *myWiFiManager)
{

    scrollingLayer.stop();
    scrollingLayer.setColor(WHITE);
    scrollingLayer.setMode(wrapForward);
    scrollingLayer.setSpeed(80);
    scrollingLayer.setFont(font6x10);
    scrollingLayer.start("Connect to Access Point:", -1);

    // Show SSID
    backgroundLayer.setFont(font5x7);
    backgroundLayer.drawString(0, 21, LIGHT_BLUE, host);
    backgroundLayer.swapBuffers();
}

StaticJsonDocument<3500> document;

char url1[100];
char url2[100];
char url3[100];
char url4[100];

bool saveConfig = false;

void saveConfigCallback()
{
    saveConfig = true;
    Serial.println(F("We need to save the config"));
}

void setup()
{
    decoder.setScreenClearCallback(screenClearCallback);
    decoder.setUpdateScreenCallback(updateScreenCallback);
    decoder.setDrawPixelCallback(drawPixelCallback);

    Serial.begin(115200);
    delay(1000);

    Serial.println(F("Mounting FS..."));

    if (SPIFFS.begin(true))
    {
        if (SPIFFS.exists("/config.json"))
        {
            Serial.println(F("FS mounted"));
            File configFile = SPIFFS.open("/config.json", "r");
            if (configFile)
            {
                size_t size = configFile.size();
                std::unique_ptr<char[]> buf(new char[size]);

                configFile.readBytes(buf.get(), size);
                DynamicJsonDocument doc(256);
                DeserializationError error = deserializeJson(doc, buf.get());
                if (error)
                {
                    Serial.println(F("There was an error reading the config.json file"));
                    return;
                }
                JsonObject json = doc.as<JsonObject>();

                serializeJson(json, Serial);
                if (!json.isNull())
                {

                    strcpy(url1, json["url1"]);
                    strcpy(url2, json["url2"]);
                    strcpy(url3, json["url3"]);
                    strcpy(url4, json["url4"]);
                }
                else
                {
                    Serial.println(F("Something happened"));
                }
            }
        }
    }
    else
    {
        // Serial.println("Couldn't mount the FS");
    }

    WiFiManagerParameter custom_url1("server", "GIF URL 1", url1, 100);
    WiFiManagerParameter custom_url2("url", "GIF URL 2", url2, 100);
    WiFiManagerParameter custom_url3("url", "GIF URL 3", url3, 100);
    WiFiManagerParameter custom_url4("url", "GIF URL 4", url4, 100);

    WiFiManager wifiManager;
    WiFi.macAddress(mac);
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(macFull, "%02X%02X%02X", baseMac[3], baseMac[4], baseMac[5]);
    // snprintf(hostName, 11, PSTR("PLM-%s"), macFull);

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
    WiFi.setHostname(host);
    WiFi.setAutoReconnect(true);

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setTimeout(180);
    wifiManager.setCleanConnect(true);
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    wifiManager.addParameter(&custom_url1);
    wifiManager.addParameter(&custom_url2);
    wifiManager.addParameter(&custom_url3);
    wifiManager.addParameter(&custom_url4);

    if (!wifiManager.autoConnect(host, "password"))
    {
        delay(3000);
    }
    scrollingLayer.stop();
    scrollingLayer.setMode(stopped);
    scrollingLayer.start(host, -1);
    delay(3000);

    Serial.print(F("Hostname: "));
    Serial.println(host);


    if (saveConfig)
    {
        strcpy(url1, custom_url1.getValue());
        strcpy(url2, custom_url2.getValue());
        strcpy(url3, custom_url3.getValue());
        strcpy(url4, custom_url4.getValue());

        DynamicJsonDocument doc(256);
        JsonObject json = doc.to<JsonObject>();

        json["url1"] = url1;
        json["url2"] = url2;
        json["url3"] = url3;
        json["url4"] = url4;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile)
        {
            Serial.println(F("There was an error opening the file to write"));
        }

        serializeJson(json, configFile);
        configFile.close();
        saveConfig = false;
    }
}

uint32_t currentFrameDelay = 5000; // ms

void loop()
{
    // Try to reconnect to wifi if connection lost
    while (WiFi.status() != WL_CONNECTED || WiFi.localIP() == IPAddress(0, 0, 0, 0))
    {
        WiFi.reconnect();
        Serial.println(F("Trying to reconnect to WiFi"));
        delay(3000);
    }

    // if (brightness > 0)
    // {
    //     matrix.setBrightness((brightness * 255) / 100);
    //     brightness = -1;
    // }
    // if (currentMode == WELCOME)
    // {
    //     scrollingLayer.setColor(WHITE);
    //     scrollingLayer.setMode(wrapForward);
    //     scrollingLayer.setSpeed(60);
    //     scrollingLayer.setFont(font6x10);
    //     scrollingLayer.start("Getting images...", -1);
    //     currentMode = APPLET;
    // }
    // if (currentMode == APPLET)
    // {
    //     scrollingLayer.stop();
    //     static uint32_t lastFrameDisplayTime = 0;
    //     unsigned long now = millis();

    //     if ((millis() - lastFrameDisplayTime) > currentFrameDelay)
    //     {
    //         if (newapplet)
    //         {
    //             decoder.startDecoding((uint8_t *)appletdecoded, outputLength);
    //             newapplet = false;
    //         }
    //         decoder.decodeFrame(false);

    //         lastFrameDisplayTime = now;
    //         currentFrameDelay = decoder.getFrameDelay_ms();
    //     }
    // }
}
