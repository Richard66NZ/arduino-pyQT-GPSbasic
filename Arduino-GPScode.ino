/* GPS location code - basic example
@date: 10 January 2021

This source code is provided by Richard J Smith 'as is' and 'with all faults'. The provider makes no 
representations or warranties of any kind concerning the safety, suitability, inaccuracies, 
typographical errors, or other harmful components of this software.

@author: Richard J Smith

The following code is only intended as a quick/rough demonstration of - using an Arduino UNO to measure GPS data, 
transfering GPS data via serial connection to a PC.
*/

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <Wire.h>

rgb_lcd lcd;

long lat, lon;
unsigned long age, time, date, speed, course;
int year;
int DisplayState = 1;
byte month, day, hour, minute, second, hundredths;
float curLAT, curLON, curSAT, curSPEED, curAGE, curALT, curCOURSE;

SoftwareSerial GPS(2, 3);
TinyGPS gps;

String incomingstring = "";
bool stringcomplete = false;
unsigned long previousMillis1 = 0;
long OnTime1 = 2000;
unsigned long previousMillis2 = 0;
long OnTime2 = 10000;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting"));
  incomingstring.reserve(200);

  lcd.begin(16, 2);

  GPS.begin(9600);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("GPSbasic"));
  lcd.setCursor(0, 1);
  lcd.print(F("Wait please"));
  int x = 0;
  do {
    Serial.println(F("GPSbasic"));
    x ++;
    delay (100);
  } while (x < 10);
}

void loop() {
  byte a;
  if (GPS.available())
  {
    a = GPS.read();
    while (gps.encode(a))
    {
      if (stringcomplete) {
        if (incomingstring.startsWith("Ping")) {
          Serial.println(F("Application GPSbasic - GPS RoyalTek"));
        }
        incomingstring = "";
        stringcomplete = false;
      }
      gps.get_position(&lat, &lon);
      gps.get_datetime(&date, &time, &age);
      gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
      curLAT = lat;
      curLON = lon;
      curSAT = gps.satellites();
      curSPEED = gps.speed();
      curALT = gps.altitude();
      curCOURSE = gps.course();
      curAGE = age;

      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis1 >= OnTime1) {
        previousMillis1 = currentMillis;
        if (DisplayState == 1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(F("Lat:  "));
          lcd.print(curLAT * 0.000001, 6);
          lcd.setCursor(0, 1);
          lcd.print(F("Long: "));
          lcd.print(curLON * 0.000001, 6);
          DisplayState = 2;
        } else if (DisplayState == 2) {
          lcd.clear();
          lcd.print(F("Sat:  "));
          lcd.print(curSAT, 0);
          DisplayState = 3;
        } else if (DisplayState == 3) {
          lcd.clear();
          lcd.print(F("Speed: "));
          lcd.print((curSPEED * 1.852), 0);
          lcd.print(F("km/h"));
          lcd.setCursor(0, 1);
          lcd.print(F("Alt: "));
          lcd.print((curALT / 100), 2);
          lcd.print(F("m"));
          DisplayState = 4;
        } else if (DisplayState == 4) {
          lcd.clear();
          lcd.print(F("Date: "));
          lcd.print(static_cast<int>(day)); lcd.print("/");
          lcd.print(static_cast<int>(month)); lcd.print("/"); lcd.print(year);
          lcd.setCursor(0, 1);
          lcd.print(F("Time: "));
          lcd.print(static_cast<int>(hour)); lcd.print(":");
          lcd.print(static_cast<int>(minute)); lcd.print(":"); lcd.print(static_cast<int>(second));
          DisplayState = 1;
        }
      }
      if (currentMillis - previousMillis2 >= OnTime2) {
        previousMillis2 = currentMillis;
        gps.get_datetime(&date, &time, &age);
        gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
        Serial.print("##");
        Serial.print(",");
        Serial.print("lat"); Serial.print(","); Serial.print((curLAT * 0.000001), 8); Serial.print(",");
        Serial.print("lon"); Serial.print(","); Serial.print((curLON * 0.000001), 8); Serial.print(",");
        Serial.print("Sat"); Serial.print(","); Serial.print((curSAT), 0); Serial.print(",");
        Serial.print("Speed(knots)"); Serial.print(","); Serial.print((curSPEED), 2); Serial.print(",");
        Serial.print("Speed(km/h)"); Serial.print(","); Serial.print((curSPEED * 1.852), 2); Serial.print(",");
        Serial.print("Alt(m)"); Serial.print(","); Serial.print((curALT / 100), 2); Serial.print(",");
        Serial.print("Course"); Serial.print(","); Serial.print(curCOURSE); Serial.print(",");
        Serial.print("Date"); Serial.print(","); Serial.print(static_cast<int>(day)); Serial.print("/");
        Serial.print(static_cast<int>(month)); Serial.print("/"); Serial.print(year);
        Serial.print(",");
        Serial.print("Time"); Serial.print(","); Serial.print(static_cast<int>(hour)); Serial.print(":");
        Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
        Serial.print(",");
        Serial.println("##");
      }
      delay(50);
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    incomingstring += inChar;
    if (inChar == '\n') {
      stringcomplete = true;
    }
  }
}
