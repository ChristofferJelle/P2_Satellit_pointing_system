#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const int pinHall = A0;
float vccActual = 4998.0; // Mål faktiske 5V pin og indsæt mV her
float zeroPoint = 511.2;  // 512.0 Standard midtpunkt (bliver kalibreret i setup)

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Kalibrering af nulpunkt (Quiescent Voltage)
  long sum = 0;
  for(int i = 0; i < 200; i++) {
    sum += analogRead(pinHall);
    delay(2);
  }
  zeroPoint = sum / 200.0;
}

void loop() {
    Serial.println(zeroPoint);
  long rawSum = 0;
  int samples = 1000;
  for (int i = 0; i < samples; i++) {
    rawSum += analogRead(pinHall);
  }
  float avgRaw = (float)rawSum / samples;
  float outputV = (avgRaw * vccActual) / 1023.0;

  float temp = dht.readTemperature();
  if (isnan(temp)) temp = 25.0;

  // beregning af følsomhed (Sensitivity)
  // Databladet siger ca. 1.8 mV/G ved 5V. 
  // Følsomheden ændrer sig med VCC: Sens = (VCC / 5.0) * 1.8
  float sensitivity = (vccActual / 5000.0) * 1.8; 

  // Beregn Magnetfelt i Gauss (G) og derefter milliTesla (mT)
  // (Aktuel spænding - Nulpunkt spænding) / følsomhed
  float zeroPointV = (zeroPoint * vccActual) / 1023.0;
  float magneticFluxG = (outputV - zeroPointV) / sensitivity;
  float magneticFluxMT = magneticFluxG / 10.0; // 10 Gauss = 1 mT

  // Output
  Serial.print("V: "); Serial.print(outputV);
  Serial.print(" mV | Temp: "); Serial.print(temp);
  Serial.print(" C | Felt: "); Serial.print(magneticFluxMT, 3);
  Serial.println(" mT");

  delay(10);
}
