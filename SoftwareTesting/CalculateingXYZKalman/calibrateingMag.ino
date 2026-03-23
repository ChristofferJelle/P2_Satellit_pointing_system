void UpdateAK09918() {
  ak09918.getData(&magX, &magY, &magZ);
  uncalibrated_values[0] = magX;
  uncalibrated_values[1] = magY;
  uncalibrated_values[2] = magZ;
  transformation(uncalibrated_values);
}

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

void UpdateYaw() {  // See: https://www.nxp.com.cn/docs/en/application-note/AN4248.pdf
 

  double rollAngle = kalAngleX * DEG_TO_RAD;
  double pitchAngle = kalAngleY * DEG_TO_RAD;

  //Tilt-Compensation Algorithm equation 19, for x,y axis
  double Bfy = magZC * sin(rollAngle) - magYC * cos(rollAngle);
  double Bfx = magXC * cos(pitchAngle) + magYC * sin(pitchAngle) * sin(rollAngle) + magZC * sin(pitchAngle) * cos(rollAngle);
  yaw = atan2(-Bfy, Bfx) * RAD_TO_DEG;


}