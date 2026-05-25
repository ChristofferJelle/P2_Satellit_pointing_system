void UpdateICM20600() {
  gyroZ = icm20600.getGyroscopeZ();
  gyroZrate = gyroZ / 32.8;  
}
