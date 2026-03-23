#include "AK09918.h"
#include "ICM20600.h"
#include <Wire.h>
#include <Kalman.h>  // Source: https://github.com/TKJElectronics/KalmanFilter

ICM20600 icm20600(true);

Kalman kalmanX, kalmanY, kalmanZ;  // Create the Kalman instances

AK09918 ak09918;
int32_t magX, magY, magZ;
float magXC, magYC, magZC;

float uncalibrated_values[3];
float calibrated_values[3];


int32_t accX, accY, accZ;
float gyroX, gyroY, gyroZ;

double roll, pitch, yaw;  // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer

double gyroXangle, gyroYangle, gyroZangle;  // Angle calculate using the gyro only
double gyroXrate, gyroYrate, gyroZrate;


double compAngleX, compAngleY, compAngleZ;  // Calculated angle using a complementary filter
double kalAngleX, kalAngleY, kalAngleZ;     // Calculated angle using a Kalman filter

uint32_t timer;

void setup() {
  int sda_pin = 7;
  int scl_pin = 8;

  Wire.setPins(sda_pin, scl_pin);
  Wire.begin();
  ak09918.initialize();
  ak09918.switchMode(AK09918_CONTINUOUS_100HZ);
  icm20600.setGyroScaleRange(RANGE_1K_DPS);
  icm20600.initialize();
Serial.begin(115200);

  /* Set Kalman and gyro starting angle */
  UpdateAK09918();
  UpdateICM20600();
  UpdatePitchRoll();
  UpdateYaw();

  kalmanX.setAngle(roll);  // First set roll starting angle
  gyroXangle = roll;
  compAngleX = roll;

  kalmanY.setAngle(pitch);  // Then pitch
  gyroYangle = pitch;
  compAngleY = pitch;

  kalmanZ.setAngle(yaw);  // And finally yaw
  gyroZangle = yaw;
  compAngleZ = yaw;

  timer = micros();  // Initialize the timer
}

//https://www.instructables.com/Easy-hard-and-soft-iron-magnetometer-calibration/
void loop() {
  UpdateAK09918();
  UpdateICM20600();

  double dt = (double)(micros() - timer) / 1000000;  // Calculate delta time
  timer = micros();

  /* Roll and pitch estimation */
  UpdatePitchRoll();

  /// This fixes the transition problem when the accelerometer angle jumps between -180 and 180 degrees
  if ((roll < -90 && kalAngleX > 90) || (roll > 90 && kalAngleX < -90)) {
    kalmanX.setAngle(roll);
    compAngleX = roll;
    kalAngleX = roll;
    gyroXangle = roll;
  } else
    kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt);  // Calculate the angle using a Kalman filter

  if (abs(kalAngleX) > 90)
    gyroYrate = -gyroYrate;  // Invert rate, so it fits the restricted accelerometer reading
  kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt);

  UpdateYaw();
  // This fixes the transition problem when the yaw angle jumps between -180 and 180 degrees
  if ((yaw < -90 && kalAngleZ > 90) || (yaw > 90 && kalAngleZ < -90)) {
    kalmanZ.setAngle(yaw);
    compAngleZ = yaw;
    kalAngleZ = yaw;
    gyroZangle = yaw;
  } else
    kalAngleZ = kalmanZ.getAngle(yaw, gyroZrate, dt);  // Calculate the angle using a Kalman filter


  /* Estimate angles using gyro only */
  //gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  //gyroYangle += gyroYrate * dt;
  //gyroZangle += gyroZrate * dt;
  gyroXangle += kalmanX.getRate() * dt;  // Calculate gyro angle using the unbiased rate from the Kalman filter
  gyroYangle += kalmanY.getRate() * dt;
  gyroZangle += kalmanZ.getRate() * dt;


  /* Estimate angles using complimentary filter */
  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll;  // Calculate the angle using a Complimentary filter
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;
  compAngleZ = 0.93 * (compAngleZ + gyroZrate * dt) + 0.07 * yaw;

  // Reset the gyro angles when they has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;
  if (gyroZangle < -180 || gyroZangle > 180)
    gyroZangle = kalAngleZ;

  /*
  Serial.flush();
  Serial.print(kalAngleX);
  Serial.print(",");
  Serial.print(kalAngleY);
  Serial.print(",");
  Serial.print(kalAngleZ);
  Serial.println();
  //magneticField();
  delay(100);
*/
  Serial.print(roll);
  Serial.print("\t");
  Serial.print(gyroXangle);
  Serial.print("\t");
  Serial.print(compAngleX);
  Serial.print("\t");
  Serial.print(kalAngleX);
  Serial.print("\t");

  Serial.print("\t");

  Serial.print(pitch);
  Serial.print("\t");
  Serial.print(gyroYangle);
  Serial.print("\t");
  Serial.print(compAngleY);
  Serial.print("\t");
  Serial.print(kalAngleY);
  Serial.print("\t");

  Serial.print("\t");

  Serial.print(yaw);
  Serial.print("\t");
  Serial.print(gyroZangle);
  Serial.print("\t");
  Serial.print(compAngleZ);
  Serial.print("\t");
  Serial.print(kalAngleZ);
  Serial.print("\t");

  Serial.println();

}