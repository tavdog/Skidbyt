#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
// #include <NtpClientLib.h>

int countDigit(long long n);
int alignToCenter (int totalWidth, int fontWidth, int digits);

// extern boolean ntpEventTriggered;
// extern NTPSyncEvent_t ntpEvent;
// extern boolean syncEventTriggered; 

// void ntpUpdate();
// void processSyncEvent(NTPSyncEvent_t ntpEvent);

byte utf8ascii(byte ascii);
String utf8ascii(String s);
void utf8ascii(char* s);

int octalToDecimal(int octal);
void printLocalTime();
struct tm now();
#endif