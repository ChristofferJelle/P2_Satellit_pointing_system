void PID() {
  double gap = abs(setpoint - kalAngleZ);  //distance away from setpoint
    //we're close to setpoint, use conservative tuning parameters
  if (gap < 50) {  
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }

    myPID.Compute();
  if (kalAngleZ > setpoint) {
  //output is caluclated from myPID.compute the closer it is to setpoint,
  // the less power coil getting, if anym when at setpoint.
    StartCoil(1, Output, 10, true);
    StartCoil(2, Output, 10, true);
  } else if (kalAngleZ < setpoint) {
    StartCoil(1, Output, 10, false);
    StartCoil(2, Output, 10, false);
    //reverse it with homebridge
  }
}
