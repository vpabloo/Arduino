#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sábado"};

//TinyGPS gps; // The serial connection to the GPS module
TinyGPSPlus gps2;
SoftwareSerial gpsSerial(10, 9); //RX, TX configuración para el GPS
//float lat = 0.0;
//float lon = 0.0; // create variable for latitude and longitude object

//global variables
unsigned long previousMillis = 0; // variable para la lectura del gps cada X tiempo
const long interval = 100000; // tiempo de intervalo de lecturas
char nombrearchivo[ ] = "TP1486.TXT"; // nombre del archivo a crear en la micro sd
const int analogPin = A0; // puerto del potenciometro
int value;      //variable que almacena la lectura analógica raw
int position;   //posicion del potenciometro en tanto por ciento

//this is for serial read
int incomingByte = 0;

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

File archivo; //variable para el archivo de la micro sd

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  SD.begin(4);
  // Open serial communications and wait for port to open:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("\nInitializing SD card... ");
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  cardinfo();
}
//-----------------------------------------------------------------------------------------
void cardinfo()
{
  // Serial.println("Información de la tarjeta");
  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }
  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }
  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());

  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);

  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}
//---------------------------------------------------------------------------------------------------
void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
//    guarda(gps);
  }
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    // say what you got:
    //    Serial.print("I received: ");
    //   Serial.println(incomingByte, DEC);
    if (incomingByte == 49) { // tecla 1
      Serial.println("\nCard info");
      cardinfo();
    }
    if (incomingByte == 50) { // tecla 2
      Serial.println("\nGPS info");
      //getgps(gps);
      getgps();
    }
    if (incomingByte == 51) { // tecla 3
      Serial.println("\nGuarda info");
//      guarda(gps);
    }
    if (incomingByte == 52) { // tecla 4
      Serial.print("\nCont archivo "); Serial.println(nombrearchivo);
   //   openfile();
    }
    if (incomingByte == 53)  { // tecla 5
      Serial.print("\nDatetime DS3231");
     // datetime();
    }
    if (incomingByte == 48)  { // tecla 0
     // borrararchivo();
    }
  }
}
//---------------------------------------------------------------------------------
void getgps()
{
  while (gpsSerial.read() > 0) {
    gps2.encode(gpsSerial.read());
    if (gps2.location.isUpdated()) {
      Serial.print("latitude = ");
      Serial.print(gps2.location.lat(), 6);
      Serial.print("longitude = ");
      Serial.print(gps2.location.lng(), 6);
    }
  }
}
/*void getgps(TinyGPS & gps)
  {
  unsigned long age;
  while (gpsSerial.available()) { // check for gps data
    char  c = gpsSerial.read();
    if (gps.encode(c)) // encode gps data
    {
      gps.f_get_position(&lat, &lon, &age); // get latitude and longitude
      Serial.print("Position: ");
      Serial.print("Lat: ");
      Serial.print(lat, 6);
      Serial.print(", ");
      Serial.print("Lon: ");
      Serial.print(lon, 6);
      Serial.print(" Fix age: "); Serial.print(age); Serial.print("ms"); Serial.print(", ");
      potenread();
      Serial.println("");
    }
  }
  gps.f_get_position(&lat, &lon, &age); // get latitude and longitude
  Serial.print("Last position: ");
  Serial.print("Lat: ");
  Serial.print(lat, 6);
  Serial.print(", ");
  Serial.print("Lon: ");
  Serial.print(lon, 6);
  Serial.print(" Fix age: "); Serial.print(age); Serial.print("ms"); Serial.print(", ");
  potenread();
  Serial.println("");
  }
//----------------------------------------------------------------------------------
void guarda(TinyGPS & gps)
{
  char c = gpsSerial.read();
  unsigned long age;
  SdFile::dateTimeCallback(datarchivo);
  archivo = SD.open(nombrearchivo, FILE_WRITE);
  if (archivo)
  {
    gps.encode(c);
    //    Serial.println(c);
    gps.f_get_position(&lat, &lon, &age); // get latitude and longitude
    archivo.print("Position: ");
    archivo.print("Lat: ");
    archivo.print(lat, 6);
    archivo.print(", ");
    archivo.print("Lon: ");
    archivo.print(lon, 6);
    archivo.print(" Fix age: "); archivo.print(age); archivo.print("ms. ");

    DateTime now = rtc.now();

    archivo.print(now.year(), DEC);
    archivo.print('/');
    archivo.print(now.month(), DEC);
    archivo.print('/');
    archivo.print(now.day(), DEC);
    archivo.print(" (");
    archivo.print(daysOfTheWeek[now.dayOfTheWeek()]);
    archivo.print(") ");
    archivo.print(now.hour(), DEC);
    archivo.print(':');
    archivo.print(now.minute(), DEC);
    archivo.print(':');
    archivo.print(now.second(), DEC);
    archivo.print(", ");
    potenguar(); // guarda info del potenciometro
    archivo.close();
    Serial.println("\nTodo guardado exitosamente");
  }
  else
  {
    Serial.print("El archivo "); Serial.print(nombrearchivo); Serial.println(" no se abrió");
  }
}
//----------------------------------------------------------------------
void openfile()
{
  // re-open the file for reading:
  archivo = SD.open(nombrearchivo);
  if (archivo) {
    // read from the file until there's nothing else in it:
    while (archivo.available()) {
      Serial.write(archivo.read());
    }
    // close the file:
    archivo.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datos.txt");
  }
}
//---------------------------------------------------------------------
void datetime()
{
  //Serial.println("Fecha y hora del DS3231");
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}
//------------------------------------------------------------------------------
void borrararchivo()
{
  Serial.println(nombrearchivo);
  if (SD.exists(nombrearchivo)) {
    SD.remove(nombrearchivo);
    Serial.print("Se ha eliminado "); Serial.print(nombrearchivo); Serial.println(" exitosamente.");
    delay(150);
  }
}
//------------------------------------------------------------------------------
// call back for file timestamps
void datetimefile(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
//-----------------------------------------------------------------------------
void potenguar()
{
  value = analogRead(analogPin);          // realizar la lectura analógica raw
  position = map(value, 0, 1023, 0, 100);  // convertir a porcentaje

  archivo.print("Comb: "); archivo.print(position); archivo.println("%");
}
void potenread()
{
  value = analogRead(analogPin);          // realizar la lectura analógica raw
  position = map(value, 0, 1023, 0, 100);  // convertir a porcentaje

  Serial.print("Comb: "); Serial.print(position); Serial.println("%");
}
//------------------------------------------------------------------------------
void datarchivo(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();

  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}*/
