
#include <SoftwareSerial.h>
#include "TinyGPS.h"
TinyGPS gps;
SoftwareSerial nss(10, 9);
long lat, lon;
unsigned long fix_age, time, date, speed, course;
unsigned long chars;
unsigned short sentences, failed_checksum;


void setup() {
  Serial.begin(9600);
}

void loop(TinyGPS & gps)
{
  unsigned long age;
  while (nss.available())
  {
    char c = nss.read();
    if (gps.encode(c))
    {
      gps.get_position(&lat, &lon, &age);
      Serial.print(lat, 6); Serial.print(", "); Serial.print(lon, 6); Serial.print(", "); Serial.println(fix_age);
      gps.get_datetime(&date, &time, &fix_age);
      Serial.print(date); Serial.print(", "); Serial.print(time); Serial.print(", "); Serial.print(fix_age);
      delay(1000);
    }
  }
}
