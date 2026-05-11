#include "AK09918.h"
#include <Wire.h>

AK09918_err_type_t err;
int32_t x, y, z;
AK09918 ak09918;

void setup() {
  Serial.begin(115200);

  // ESP32 I2C pins
  const int sda_pin = 43;
  const int scl_pin = 44;

  Wire.setPins(sda_pin, scl_pin);
  Wire.begin();

  err = ak09918.initialize();

  if (err != AK09918_ERR_OK) {
    Serial.println("Sensor init failed");
    while (1);
  }

  ak09918.switchMode(AK09918_POWER_DOWN);
  delay(100);
  ak09918.switchMode(AK09918_NORMAL);
}

void loop() {
  err = ak09918.getData(&x, &y, &z);

  if (err == AK09918_ERR_OK) {
    // Format: X,Y,Z
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(",");
    Serial.println(z); // IMPORTANT: newline
  }

  delay(20); // ~50 Hz update rate
}