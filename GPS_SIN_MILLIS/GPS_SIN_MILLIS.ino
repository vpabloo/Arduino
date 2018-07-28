#include <SoftwareSerial.h>
#include <TinyGPS.h>

int incomingByte = 0;
SoftwareSerial mySerial(10, 9); // RX, TX
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(1000);
  Serial.print("Sizeof(gpsobject) = ");
  Serial.println(sizeof(TinyGPS));
  Serial.println();
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    // say what you got:
    // Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    if (incomingByte == 50) {
      gpsread();
    }
  }
}
//--------------------------------------------
void gpsread() {
  if (mySerial.available()) {
    char c = mySerial.read();
    if (gps.encode(c)) {
      Serial.println("Acquired Data");
      Serial.println("-------------");
      gpsdump(gps);
      Serial.println("-------------");
      Serial.println();
    }
  }
}
//------------------------------
void gpsdump(TinyGPS & gps) {
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;
  
  gps.get_position(&lat, &lon, &age);
  Serial.print("Lat/Long(10^-5 deg): ");
  Serial.print(lat);
  Serial.print(", ");
  Serial.print(lon);
  Serial.print(" Fix age: ");
  Serial.print(age);
  Serial.println("ms.");
  // On Arduino, GPS characters may be lost during lengthy Serial.print()
  // On Teensy, Serial prints to USB, which has large output buffering and
  // runs very fast, so it's not necessary to worry about missing 4800 baud GPS characters.
 
  gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): ");
  printFloat(flat, 5);
  Serial.print(", ");
  printFloat(flon, 5);
  Serial.print(" Fix age: ");
  Serial.print(age);
  Serial.println("ms.");
  
  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): ");
  Serial.print(date);
  Serial.print(" Time(hhmmsscc): ");
  Serial.print(time);
  Serial.print(" Fix age: ");
  Serial.print(age);
  Serial.println("ms.");
  Serial.print("Alt(cm): ");
  Serial.print(gps.altitude());
  Serial.print(" Course(10^-2 deg): ");
  Serial.print(gps.course());
  Serial.print(" Speed(10^-2 knots): ");
  Serial.println(gps.speed());
  Serial.print("Alt(float): ");
  printFloat(gps.f_altitude());
  Serial.print(" Course(float): ");
  printFloat(gps.f_course());
  Serial.println();
  Serial.print("Speed(knots): ");
  printFloat(gps.f_speed_knots());
  Serial.print(" (mph): ");
  printFloat(gps.f_speed_mph());
  Serial.print(" (mps): ");
  printFloat(gps.f_speed_mps());
  Serial.print(" (kmph): ");
  printFloat(gps.f_speed_kmph());
  Serial.println();
  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: ");
  Serial.print(chars);
  Serial.print(" sentences: ");
  Serial.print(sentences);
  Serial.print(" failed checksum: ");
  Serial.println(failed);
}

void printFloat(double number, int digits) {
  // Handle negative numbers
  if (number < 0.0) {
    Serial.print('-');
    number = -number;
  }
  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i = 0; i < digits; ++i)
    rounding /= 10.0;
  number += rounding;
  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);
  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print(".");
  // Extract digits from the remainder one at a time
  while (digits-- > 0) {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}
