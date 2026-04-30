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

float gyroZ;
double gyroZrate;
double yaw;  // Roll and pitch are calculated using the accelerometer while yaw is calculated using the magnetometer
//double roll, pitch,
double kalAngleZ;  // Calculated angle using a Kalman filter
//double kalAngleX, kalAngleY,
uint32_t timer;

double dt;

//PID
//#define PIN_INPUT 0
//#define PIN_OUTPUT 3


double setpoint = 180.0, Output;
//Define the aggressive and conservative Tuning Parameters
double aggKp = 4, aggKi = 0.2, aggKd = 1;
//double consKp = 1, consKi = 0.05, consKd = 0.25;
double consKp = 0.5, consKi = 0.0025, consKd = 0.125;
//Specify the links and initial tuning parameters
PID myPID(&kalAngleZ, &Output, &setpoint, consKp, consKi, consKd, DIRECT);

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

// ===========================TFT============================
//for testing pointing system only
/**/
#include <TFT_eSPI.h>  // LILYGO T-Display library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Create TFT object+

// number field positions/sizes for tft refrech areas
const int spX = 120;
const int spY = 0;
const int spW = 80;
const int spH = 16 * 2;

const int akX = 160;
const int akY = 30;
const int akW = 100;
const int akH = 16 * 2;

const int PIDX = 120;
const int PIDY = 60;
const int PIDW = 80;
const int PIDH = 16 * 2;

void setup() {

  int sda_pin = 43;
  int scl_pin = 44;

  Wire.setPins(sda_pin, scl_pin);
  Wire.begin();
  ak09918.initialize();
  ak09918.switchMode(AK09918_POWER_DOWN);
  ak09918.switchMode(AK09918_NORMAL); //insted of using Continuous measurement mode only power magometer when calling getData()


  icm20600.setGyroScaleRange(RANGE_1K_DPS);
  icm20600.initialize();

  InitDisplay();

  Serial.begin(9600);

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

  //Serial.println(WiFi.macAddress());
  /*
  Serial.print("[DEFAULT] ESP32 Board MAC Address: ");
  uint8_t* ownMac = readMacAddress();
  //converts array into a string.
  if (ownMac != nullptr) {
    String ownMacHex;
    for (int i = 0; i < 6; i++) {
      String hexPart = String(ownMac[i], HEX);
      if (hexPart.length() < 2) hexPart = "0" + hexPart;  // zero-pad if needed
      ownMacHex += hexPart;
      if (i < 5) ownMacHex += ":";  // add ':' except after last byte
    }
    Serial.println(ownMacHex);
  }
*/
  timer = micros();  // Initialize the timer
}

void loop() {
  dt = (double)(micros() - timer) / 1000000;  // Calculate delta time
  timer = micros();
  RunCoils();
}