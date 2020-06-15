#define CAL_BUTTON 2
#define MAX_BUTTON 3
#define BRIGHTNESS_PIN 10
#define LDR A0
#define LDR_MIN_THRES 200
#define LDR_MAX_THRES 800
//#define CA_7SDU
#define CC_7SDU

#include "ShiftRegister74HC595.h"
#include "I2Cdev.h"
#include "EEPROM.h"
#include "MPU6050_6Axis_MotionApps_V6_12.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

ShiftRegister74HC595<2> disp(5, 6, 7);
MPU6050 IMU;

// MPU control/status vars
bool IMUReady = false;  // set true if DMP init was successful
uint8_t IMUdevStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t IMUpacketSize;    // expected DMP packet size (default is 42 bytes)
uint16_t IMUfifoCount;     // count of all bytes currently in FIFO
uint8_t IMUfifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion IMUq;               // [w, x, y, z] quaternion container
VectorFloat IMUgravity;        // [x, y, z]  gravity vector
float IMUypr[3] = {0, 0, 0};   // [yaw, pitch, roll] yaw/pitch/roll container and gravity vector

int xAccelOffset, yAccelOffset, zAccelOffset, xGyroOffset, yGyroOffset, zGyroOffset;
String serialStr;
double angle, maxAngle = 0;
bool calTriggered = false, maxTriggered = false;
unsigned long buttonTime;

void setup() {
#ifdef CA_7SDU
  disp.setAllHigh();
#endif

#ifdef CC_7SDU
  disp.setAllLow();
#endif

TCCR1B = TCCR1B & B11111000 | B00000001;  //pin 9+10 pwm freq 31.372 KHz
//TCCR1B = TCCR1B & B11111000 | B00000100;  //pin 9+10 pwm freq 122.55 Hz
//TCCR1B = TCCR1B & B11111000 | B00000101;  //pin 9+10 pwm freq 30.64 Hz

  serialStr.reserve(40);
  Serial.begin(115200);

  pinMode(CAL_BUTTON, INPUT_PULLUP);
  pinMode(MAX_BUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESS_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(CAL_BUTTON), calButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(MAX_BUTTON), maxButton, FALLING);

  eeprom_get();

  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock

  //initialise IMU
  IMUsetup();

  Serial.println("Ready!");

  analogWrite(BRIGHTNESS_PIN, 25);

  //reset IMU FIFOs for clean start
  IMU.resetFIFO();
}

void loop() {
  // if IMU programming failed, don't try to do anything
  if (!IMUReady) return;  //halt if MPU is missing

  IMUdata();
//  Serial.println(angle);
  updateDisp(abs(angle));

  if (abs(angle) > maxAngle) {
    maxAngle = abs(angle);
  }

  brightness();

  calCheck();
  maxCheck();
}

void calButton() {
  calTriggered = true;
}

void maxButton() {
  maxTriggered = true;
}
