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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(Coil1Fwd, OUTPUT);
  pinMode(Coil1Rvr, OUTPUT);
  pinMode(Coil2Fwd, OUTPUT);
  pinMode(Coil2Rvr, OUTPUT);
  coils[0].rvrPin = Coil1Rvr;
  coils[0].fwdPin = Coil1Fwd;
  coils[1].rvrPin = Coil2Rvr;
  coils[1].fwdPin = Coil2Fwd;

  // StartCoil(1, 255, 120000, true);
}

uint32_t var1;
uint32_t var2;
uint32_t var3;

void loop() {
  // put your main code here, to run repeatedly:
  RunCoils();
  // if (millis() % 4000 >= 2000) {
  //     StartCoil(1, 255, 100, true);
  // } else {
  //     StartCoil(1, 255, 100, false);
  // }
 
  if (millis() % 4000 >= 2000) {
    if (millis() % 2000 >= 1000) {
      var1 = millis() % 1000;
      var1 = map(var1, 0, 999, 255, 0);
    }
    else {
      var1 = millis() % 1000;
      var1 = map(var1, 0, 999, 0, 255);
    }
    StartCoil(1, var1, 10, true);
  }
  else {
    if (millis() % 2000 >= 1000) {
      var1 = millis() % 1000;
      var1 = map(var1, 0, 999, 255, 0);
    }
    else {
      var1 = millis() % 1000;
      var1 = map(var1, 0, 999, 0, 255);
    }
    StartCoil(1, var1, 10, false);
  }
}

void StartCoil(int coilNumber, int power, uint32_t duration, bool forward) {
  coils[coilNumber].startTime = millis();
  coils[coilNumber].runTime = duration;
  coils[coilNumber].power = power;
  coils[coilNumber].running = true;
  coils[coilNumber].forward = forward;
  analogWrite(coils[coilNumber].fwdPin, 0);
  analogWrite(coils[coilNumber].rvrPin, 0);
  coils[coilNumber].hasInput = false;
}

void RunCoils() {
  for (int i = 0; i <= 1; i++) {
    if (coils[i].running && !coils[i].hasInput) {
      if (coils[i].forward) {
        analogWrite(coils[i].fwdPin, coils[i].power);
        coils[i].hasInput = true;
      } else {
        analogWrite(coils[i].rvrPin, coils[i].power);
        coils[i].hasInput = true;
      }
    }
    if (coils[i].running && coils[i].hasInput && millis() - coils[i].startTime >= coils[i].runTime) {
      analogWrite(coils[i].fwdPin, 0);
      analogWrite(coils[i].rvrPin, 0);
      coils[i].running = false;
      coils[i].hasInput = false;
    }
  }
}
