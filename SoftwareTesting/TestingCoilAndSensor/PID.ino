
void PID() {
  double gap = abs(Setpoint - kalAngleZ);  //distance away from setpoint

  if (gap < 50) {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }


  if (kalAngleZ > 0) {
    myPID.Compute(false);
    analogWrite(Coil1Fwd, Output);
  } else if (kalAngleZ < 0) {
    myPID.Compute(true);
    //reverse it homebridge go brr ig?
    //analogWrite(PIN2_OUTPUT, Output);
  }
}
