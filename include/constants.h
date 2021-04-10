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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

#define NTP_SERVER "es.pool.ntp.org"
const float hourOffset = 1;
#define MQTT_CLIENT "panel-led-mqtt"
#define STATUS_TOPIC "plm/status"
#define APPLET_TOPIC "plm/applet"
#define BRIGHTNESS_TOPIC "plm/brightness"


#define BLACK           {0x00,0x00,0x00}
#define WHITE           {0xFF,0xFF,0xFF}
#define BLUE            {0x00,0x00,0xFF}
#define YELLOW          {0xFF,0xFF,0x00}
#define GREEN           {0x00,0xFF,0x00}
#define MAGENTA         {0xFF,0x00,0xFF}
#define RED             {0xFF,0x00,0x00}
#define CYAN            {0x00,0xFF,0xFF}

#define GRAY            {0x80,0x80,0x80}
#define LIGHT_GRAY      {0xC0,0xC0,0xC0}
#define PALE_GRAY       {0xE0,0xE0,0xE0}
#define DARK_GRAY       {0x40,0x40,0x40}

#define DARK_BLUE       {0x00,0x00,0x80}
#define DARK_GREEN      {0x00,0x80,0x00}
#define DARK_RED        {0x80,0x00,0x00}
#define LIGHT_BLUE      {0x80,0xC0,0xFF}
#define LIGHT_GREEN     {0x80,0xFF,0x80}
#define LIGHT_RED       {0xFF,0xC0,0xFF}
#define PINK            {0xFF,0xAF,0xAF}
#define BROWN           {0x60,0x30,0x00}
#define ORANGE          {0xFF,0x80,0x00}
#define PURPLE          {0xC0,0x00,0xFF}
#define LIME            {0x80,0xFF,0x00}

// Modos
const int WELCOME = 1;
const int APPLET = 2;
const int NONE = 0;

#endif