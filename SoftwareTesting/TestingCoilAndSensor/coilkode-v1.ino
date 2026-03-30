

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
