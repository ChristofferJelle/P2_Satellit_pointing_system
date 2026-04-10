#include "src/Grove_IMU_9DOF_ICM20600_AK09918/AK09918.h"  //https://github.com/Seeed-Studio/Seeed_ICM20600_AK09918
#include "src/Grove_IMU_9DOF_ICM20600_AK09918/ICM20600.h"
#include <Wire.h>
#include <Kalman.h>  // Source: https://github.com/TKJElectronics/KalmanFilter
#include "src/PID/PID_v1.h"

ICM20600 icm20600(true);

Kalman kalmanZ;  // Create the Kalman instances
//Kalman kalmanX, kalmanY,
AK09918 ak09918;
int32_t magX, magY, magZ;
float magXC, magYC, magZC;

float uncalibrated_values[3];
float calibrated_values[3];

//int32_t accX, accY, accZ;
float gyroX, gyroY, gyroZ;
double gyroXrate, gyroYrate, gyroZrate;
double  yaw;  // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer
//double roll, pitch,
double kalAngleZ;  // Calculated angle using a Kalman filter
//double kalAngleX, kalAngleY,
uint32_t timer;

double dt;

//PID
//#define PIN_INPUT 0
//#define PIN_OUTPUT 3

//Define Variables we'll be connecting to
double Setpoint = 180, Output;
//Define the aggressive and conservative Tuning Parameters
double aggKp = 4, aggKi = 0.2, aggKd = 1;
//double consKp = 1, consKi = 0.05, consKd = 0.25;
double consKp = 0.5, consKi = 0.0025, consKd = 0.125;
//Specify the links and initial tuning parameters
PID myPID(&kalAngleZ, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);

//COILS ============================================================================

#define Coil1Fwd 3
#define Coil1Rvr 9
#define Coil2Fwd 10
#define Coil2Rvr 11

typedef struct {
  bool running = false;
  uint32_t startTime = 0;
  uint32_t runTime = 0;
  int power = 0;
  bool hasInput = false;
  int rvrPin = 0;
  int fwdPin = 0;
  bool forward = false;
} coil;

coil coils[2];

int sampleTime = 5000;  // 500 milis in marcro
int lastTime = timer - 5000;

// ===========================ESPNOW============================
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

void setup() {
  int sda_pin = 21;
  int scl_pin = 22;

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
  //UpdatePitchRoll();
  UpdateYaw();

  //kalmanX.setAngle(roll);
  //kalmanY.setAngle(pitch);
  kalmanZ.setAngle(yaw);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(10);

  // =========================COILS====================================
  pinMode(Coil1Fwd, OUTPUT);
  pinMode(Coil1Rvr, OUTPUT);
  pinMode(Coil2Fwd, OUTPUT);
  pinMode(Coil2Rvr, OUTPUT);
  coils[0].rvrPin = Coil1Rvr;
  coils[0].fwdPin = Coil1Fwd;
  coils[1].rvrPin = Coil2Rvr;
  coils[1].fwdPin = Coil2Fwd;

  //=========================ESPNOW=============================
  //pinMode(BUTTON_PIN, INPUT);
  InitESP32_NOW();
  Serial.println(WiFi.macAddress());

  timer = micros();  // Initialize the timer
}

void loop() {
  UpdateICM20600();
  dt = (double)(micros() - timer) / 1000000;  // Calculate delta time
  timer = micros();
  /* Roll and pitch estimation for jew tilt compensation */
  //UpdatePitchRoll();
 //CalculateXYangle();

  RunCoils();
  //delay(100);
}