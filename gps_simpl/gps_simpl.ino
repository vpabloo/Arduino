#include <SoftwareSerial.h>
#include <TinyGPS.h>
float lat = 0.0, lon = 0.0; // create variable for latitude and longitude object
SoftwareSerial gpsSerial(10, 9); //rx,tx
TinyGPS gps; // create gps object
int lecturaserial = 0;

void setup() {
  Serial.begin(9600); // connect serial
  Serial.println("The GPS Received Signal:");
  gpsSerial.begin(9600); // connect gps sensor
}
void loop() {
  if (Serial.available() > 0) {
    lecturaserial = Serial.read();
    Serial.println(lecturaserial, DEC);
    if (lecturaserial == 49) {
      getgps(gps);
    }
  }
}
//-------------------------------
void getgps(TinyGPS & gps)
{
  unsigned long age;
  while (gpsSerial.available() > 0) { // check for gps data
    char  c = gpsSerial.read();
    if (gps.encode(c)) // encode gps data
    {
      gps.f_get_position(&lat, &lon, &age); // get latitude and longitude
      Serial.print("Position: ");
      Serial.print("Latitude: ");
      Serial.print(lat, 5);
      Serial.print(", ");
      Serial.print("Longitude: ");
      Serial.print(lon, 5);
      Serial.print(", ");
      Serial.println(age);
    }
  }
  gps.f_get_position(&lat, &lon, &age); // get latitude and longitude
  Serial.print("Last position: ");
  Serial.print("Latitude: ");
  Serial.print(lat, 5);
  Serial.print(", ");
  Serial.print("Longitude: ");
  Serial.print(lon, 5);
  Serial.print(", ");
  Serial.println(age);
  /*String latitude = String(lat, 6);
    String longitude = String(lon, 6);
    Serial.println(latitude + "," + longitude);
  */
}
