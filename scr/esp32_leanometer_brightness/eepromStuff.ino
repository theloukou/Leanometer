void eeprom_get() {
  int eeAddress = 0;
  EEPROM.get(eeAddress, xAccelOffset);
  eeAddress += sizeof(int);
  EEPROM.get(eeAddress, yAccelOffset);
  eeAddress += sizeof(int);
  EEPROM.get(eeAddress, zAccelOffset);
  eeAddress += sizeof(int);
  EEPROM.get(eeAddress, xGyroOffset);
  eeAddress += sizeof(int);
  EEPROM.get(eeAddress, yGyroOffset);
  eeAddress += sizeof(int);
  EEPROM.get(eeAddress, zGyroOffset);
}

void eeprom_put() {
  int eeAddress = 0;
  EEPROM.put(eeAddress, IMU.getXAccelOffset());
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, IMU.getYAccelOffset());
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, IMU.getZAccelOffset());
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, IMU.getXGyroOffset());
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, IMU.getYGyroOffset());
  eeAddress += sizeof(int);
  EEPROM.put(eeAddress, IMU.getZGyroOffset());
}
