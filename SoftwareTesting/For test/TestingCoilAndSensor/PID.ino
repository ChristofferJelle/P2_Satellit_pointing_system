void PID() {
  double gap = abs(setpoint - kalAngleZ);  //distance away from setpoint

  if (gap < 50) {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }

    myPID.Compute();
  if (kalAngleZ > setpoint) { // dont know ifs im using both coils correctly here 
  //output is caluclated from myPID.compute the closer it is to setpoint, the less power coil getting if any when at setpoint.
    StartCoil(1, Output, 10, true);
    StartCoil(2, Output, 10, true);
  } else if (kalAngleZ < setpoint) {
    StartCoil(1, Output, 10, false);
    StartCoil(2, Output, 10, false);
    //reverse it homebridge go brr ig?
    //analogWrite(PIN2_OUTPUT, Output);
  }
}
