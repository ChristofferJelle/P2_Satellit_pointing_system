void UpdateICM20600() {
  accX = icm20600.getAccelerationX();
  accY = icm20600.getAccelerationY();
  accZ = icm20600.getAccelerationZ();

  gyroX = icm20600.getGyroscopeX();
  gyroY = icm20600.getGyroscopeY();
  gyroZ = icm20600.getGyroscopeZ();
  gyroXrate = gyroX / 32.8;  // Convert to deg/s
  gyroYrate = gyroY / 32.8;  // Convert to deg/s
  gyroZrate = gyroZ / 32.8;  // Convert to deg/s
}

//Source: https://www.nxp.com.cn/docs/en/application-note/AN3461.pdf eq 25 and 26
// atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
// It is then converted from radians to degrees

void UpdatePitchRoll() {
  // Source: http://www.freescale.com/files/sensors/doc/app_note/AN3461.pdf
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // It is then converted from radians to degrees
  // Eq. 25 and 26
  roll = atan2(accY, accZ) * RAD_TO_DEG;
  pitch = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
}
