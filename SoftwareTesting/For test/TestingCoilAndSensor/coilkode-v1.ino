//startCoil(CoilID, pwm, timeTurnedOn, Hbro vending)
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
  unsigned long now = micros();
  unsigned long timeChange = (now - lastTime);

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
      lastTime = now;  //should only run once, when the coils done moving
    }
    if (!(coils[0].running && coils[1].running) && (timeChange >= sampleTime)) {  //use magnetometer after 0.4 sek of coils turning off
      UpdateAK09918();
      UpdateICM20600();
      /* Yaw estimation for Z angle */
      UpdateYaw();
      calculateZAngle();
      PID();  //pid starts coils again
      DrawDisplay();
      SendDataToMaster();
      lastTime = now;
      
    }
  }
}