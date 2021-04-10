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