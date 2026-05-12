
void PID() {
  double gapNew = abs(setpoint - kalAngleZ);  //distance away from setpoint

  if (gapNew < 50) {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  myPID.Compute();
  if (kalAngleZ > setpoint) {  // dont know ifs im using both coils correctly here
    //output is caluclated from myPID.compute the closer it is to setpoint, the less power coil getting if any when at setpoint.
    if (gap > gapNew) {
      StartCoil(1, Output, 10, true);
      StartCoil(2, Output, 10, false);
    } else {
      StartCoil(1, Output, 10, false);
      StartCoil(2, Output, 10, true);
    }
  } else if (kalAngleZ < setpoint) {
    if (gap > gapNew) {
      StartCoil(1, Output, 10, false);
      StartCoil(2, Output, 10, true);
    } else {
      StartCoil(1, Output, 10, true);
      StartCoil(2, Output, 10, false);
    }
  }
  gap = gapNew;
}
