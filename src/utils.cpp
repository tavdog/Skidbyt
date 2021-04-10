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

#include <utils.h>
#include <constants.h>

// Función para contar el número de dígitos.
int countDigit(long long n) { 
    int count = 0; 
    while (n != 0) { 
        n = n / 10; 
        ++count; 
    } 
    return count; 
}

// Sí, las matemáticas del colegio, sirven para algo.
int alignToCenter (int totalWidth, int fontWidth, int digits) {
    int startAt = (totalWidth / 2) - ((fontWidth * digits) / 2);

    return startAt;
}

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
//   Serial.println(&timeinfo, "%M");
Serial.println("--------\n");
Serial.println(timeinfo.tm_min);
}

struct tm now() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
    }
    return timeinfo;
}


// Cliente NTP
// boolean ntpEventTriggered = false;
// NTPSyncEvent_t ntpEvent;
// boolean ntpSignalSent = false;
// boolean syncEventTriggered = false; 

// Actualizamos la hora
// void ntpUpdate() {
// 	Serial.println("Actualizando hora");
// 	if (!ntpSignalSent) {
// 		NTP.begin(NTP_SERVER, 0, true, 0);
// 		NTP.setInterval(3600);
// 		ntpSignalSent = true;
// 	}

// }

// Sincronizamos los eventos
// void processSyncEvent(NTPSyncEvent_t ntpEvent) {
// 	if (ntpEvent) {
// 		Serial.print("Error en la sincronización de la hora: ");
// 		if (ntpEvent == noResponse)
// 			Serial.println("No puedo contactar con el Servidor NTP");
// 		else if (ntpEvent == invalidAddress)
// 			Serial.println("Dirección del Servidor NTP no válida");
// 			else
// 			Serial.println("Error desconocido");
// 	}
// 	else {
// 		Serial.print("Hemos obtenido la hora NTP: ");
// 		Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
// 	}
// }

static byte c1;  // Último caracter del buffer
byte utf8ascii(byte ascii) {
    if (ascii<128) {
    	c1 = 0;
        return(ascii);
    }

    byte last = c1;
    c1 = ascii;

    switch (last) {
       case 0xC2: return(ascii);  break;
        case 0xC3: return(ascii | 0xC0);  break;
        case 0x82: if(ascii == 0xAC) return(0x80);
    }

    return(0);
}

String utf8ascii(String s) {      
        String r = "";
        char c;
        for (int i = 0; i < s.length(); i++) {
                c = utf8ascii(s.charAt(i));
                if (c != 0) r += c;
        }
        return r;
}

void utf8ascii(char* s) {      
        int k = 0;
        char c;
        for (int i = 0; i < strlen(s); i++) {
                c = utf8ascii(s[i]);
                if (c != 0)
                     s[k++] = c;
        }
        s[k] = 0;
}

int octalToDecimal(int octal) {
    int decimal = 0, i = 0, remainder;
    while (octal != 0) {
        remainder = octal % 10;
        octal /= 10;
        decimal += remainder * pow(8, i);
        ++i;
    }
    return decimal;
}