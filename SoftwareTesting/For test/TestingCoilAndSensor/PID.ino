void PID() {
  double gapNew = abs(setpoint - kalAngleZ);  //distance away from setpoint
  double limitValue = 50;
  /*
  if (gapNew < limitValue) {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myPID.SetTunings(aggKp, aggKi, aggKd);
  }
*/
  // deadband
  if (kalAngleZ > setpoint) {  // dont know ifs im using both coils correctly here
    //output is caluclated from myPID.compute the closer it is to setpoint, the less power coil getting if any when at setpoint.
    if (gap > gapNew) {
      if (gapNew < limitValue) {
        //clockwise
        my1PID.Compute();
        StartCoil(1, Output, 10, false);
        StartCoil(2, Output, 10, true);
      } else {
        //counter-clockwise
        my2PID.Compute();
        StartCoil(1, Output2, 10, true);
        StartCoil(2, Output2, 10, false);
      }
    }
  } else if (kalAngleZ < setpoint) {
    if (gap > gapNew) {
      if (gapNew < limitValue) {
        //counter-clockwise
        my1PID.Compute();
        StartCoil(1, Output, 10, true);
        StartCoil(2, Output, 10, false);
      } else {
        //clockwise
        my2PID.Compute();
        StartCoil(1, Output2, 10, false);
        StartCoil(2, Output2, 10, true);
      }
    }
  }
  gap = gapNew;
}
