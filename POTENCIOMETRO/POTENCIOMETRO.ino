const int analogPin = A0;
int value;      //variable que almacena la lectura analógica raw
int position;   //posicion del potenciometro en tanto por ciento

void setup() {
  Serial.begin(9600);
}

void loop() {
  value = analogRead(analogPin);          // realizar la lectura analógica raw
  position = map(value, 0, 1023, 0, 100);  // convertir a porcentaje

  Serial.print("Valor del potencionmetro: "); Serial.print(position);Serial.println("%");

  delay(1000);
}
