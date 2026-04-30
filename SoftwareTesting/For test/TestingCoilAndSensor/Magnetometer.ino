void UpdateAK09918() {
  ak09918.getData(&magX, &magY, &magZ);
  uncalibrated_values[0] = magX;
  uncalibrated_values[1] = magY;
  uncalibrated_values[2] = magZ;
  transformation(uncalibrated_values);
}

//https://www.instructables.com/Easy-hard-and-soft-iron-magnetometer-calibration/
void transformation(float uncalibrated_values[3]) {
  //calibration_matrix[3][3] is the transformation matrix
  //soft iron calibartion
  double calibration_matrix[3][3] = {
    { 1.283, -0.061, -0.02 },
    { 0.053, 1.275, 0.115 },
    { 0.067, -0.097, 1.231 }
  };
  //bias[3] is the bias
  //replace Bx, By, Bz with your bias data
  // hard iron calibration
  double bias[3] = {
    3.263,
    8.404,
    27.814
  };
  //calculation
  for (int i = 0; i < 3; ++i) uncalibrated_values[i] = uncalibrated_values[i] - bias[i];
  float result[3] = { 0, 0, 0 };
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      result[i] += calibration_matrix[i][j] * uncalibrated_values[j];
  for (int i = 0; i < 3; ++i) calibrated_values[i] = result[i];
  magXC = calibrated_values[0];
  magYC = calibrated_values[1];
  magZC = calibrated_values[2];
}

void UpdateYaw() { 
  yaw = atan2(magYC, magXC) * RAD_TO_DEG;
}
void calculateZAngle() {
  // This fixes the transition problem when the yaw angle jumps between -180 and 180 degrees
  if ((yaw < -90 && kalAngleZ > 90) || (yaw > 90 && kalAngleZ < -90)) {
    kalmanZ.setAngle(yaw);
    kalAngleZ = yaw;
  } else
    kalAngleZ = kalmanZ.getAngle(yaw, gyroZrate, dt);  // Calculate the angle using a Kalman filter
}
