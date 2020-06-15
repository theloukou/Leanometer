void IMUsetup() {
  // initialize device
  IMU.initialize();

  // verify connection
  Serial.println(IMU.testConnection() ? F("IMU connection successful") : F("IMU connection failed"));

  // load and configure the DMP
  IMUdevStatus = IMU.dmpInitialize();

  // set IMU offsets
  IMU.setXAccelOffset(xAccelOffset);
  IMU.setYAccelOffset(yAccelOffset);
  IMU.setZAccelOffset(zAccelOffset);
  IMU.setXGyroOffset(xGyroOffset);
  IMU.setYGyroOffset(yGyroOffset);
  IMU.setZGyroOffset(zGyroOffset);
  IMU.PrintActiveOffsets();

  // make sure it worked (returns 0 if so)
  if (IMUdevStatus == 0) {
    // turn on the DMP, now that it's ready
    IMU.setDMPEnabled(true);

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("IMU DMP ready!"));
    IMUReady = true;

    // get expected DMP packet size for later comparison
    IMUpacketSize = IMU.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("IMU DMP Initialization failed (code "));
    Serial.print(IMUdevStatus);
    Serial.println(F(")"));
  }
}

void IMUcalibration() {
  Serial.println("Calibrating");
  delay(3000);
  IMU.CalibrateAccel(6);
  IMU.CalibrateGyro(6);
  IMU.PrintActiveOffsets();
  eeprom_put();
}

//get IMU data, calculate needed angles
void IMUdata() {
  // get current IMU FIFO count
  IMUfifoCount = IMU.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if (IMUfifoCount >= 1024) {
    // reset so we can continue cleanly
    IMU.resetFIFO();
    Serial.println(F("IMU FIFO overflow!"));
  }
  // otherwise, check for DMP data ready interrupt (this should happen frequently)
  else if (IMUfifoCount >= 42) {
    // read a packet from FIFO
    IMU.getFIFOBytes(IMUfifoBuffer, IMUpacketSize);

    // calculate Euler angles in degrees
    IMU.dmpGetQuaternion(&IMUq, IMUfifoBuffer);
    IMU.dmpGetGravity(&IMUgravity, &IMUq);
    IMU.dmpGetYawPitchRoll(IMUypr, &IMUq, &IMUgravity);
  }

  //get desired used angles
  angle = IMUypr[2] * 180 / M_PI;
}
