#include "AK09918.h"
#include "ICM20600.h"
#include <Wire.h>
#include <Kalman.h>  // Source: https://github.com/TKJElectronics/KalmanFilter
#include "src/PID/PID_v1.h"

ICM20600 icm20600(true);

Kalman kalmanX, kalmanY, kalmanZ;  // Create the Kalman instances

AK09918 ak09918;
int32_t magX, magY, magZ;
float magXC, magYC, magZC;

float uncalibrated_values[3];
float calibrated_values[3];

int32_t accX, accY, accZ;
float gyroX, gyroY, gyroZ;
double gyroXrate, gyroYrate, gyroZrate;
double roll, pitch, yaw;  // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer

double kalAngleX, kalAngleY, kalAngleZ;  // Calculated angle using a Kalman filter

uint32_t timer;

double dt;

//PID
#define PIN_INPUT 0
#define PIN_OUTPUT 3

//Define Variables we'll be connecting to
double Setpoint = 180, Output;
//Define the aggressive and conservative Tuning Parameters
double aggKp = 4, aggKi = 0.2, aggKd = 1;
//double consKp = 1, consKi = 0.05, consKd = 0.25;
double consKp = 0.5, consKi = 0.0025, consKd = 0.125;
//Specify the links and initial tuning parameters
PID myPID(&kalAngleZ, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);


void setup() {
  int sda_pin = 43;
  int scl_pin = 44;

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

  kalmanX.setAngle(roll);
  kalmanY.setAngle(pitch);
  kalmanZ.setAngle(yaw);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(10);

  timer = micros();  // Initialize the timer
}

void loop() {
  UpdateICM20600();
  dt = (double)(micros() - timer) / 1000000;  // Calculate delta time
  timer = micros();

  /* Roll and pitch estimation for jew tilt compensation */
  UpdatePitchRoll();
  CalculateXYangle();
  /* Yaw estimation for Z angle */
  UpdateAK09918();
  UpdateYaw();
  calculateZAngle();
  
  Serial.flush();
  PID();
  /*
  Serial.print(kalAngleX);
  Serial.print(",");
  Serial.print(kalAngleY);
  Serial.print(",");
   
 */
  Serial.print(kalAngleZ);
  Serial.print(",");
  Serial.print(Output);

  Serial.println();

  //delay(100);
}