// Libraries
//#include <LowPower.h>
#include <Wire.h>

// Constants
#define ALARM_PIN 13

// Variables
const int MPU_addr = 0x68;  // I2C address of the MPU-6050
bool alarm = false;
int16_t AcX, AcY, AcZ; // Acceleration values
float baseline;
int sensitivity = 5000; // Smaller numbers are more sensitive
int magnitude;

// Functions

// SOUND THE ALARM
void soundAlarm() {
  alarm = true;
  digitalWrite(ALARM_PIN, HIGH);
}

// Main
// Setup
void setup(){
  // Set pins
  pinMode(ALARM_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW);

  // Accellerometer setup
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

// Loop
void loop(){
  // If alarm has not yet gone off
  if(!alarm) {

      // Get info from accelerometer
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);
      Wire.requestFrom(MPU_addr, 6, true); // request a total of 6 registers

      // Read acceleration values
      AcX = Wire.read()<<8|Wire.read(); // X acceleration
      AcY = Wire.read()<<8|Wire.read(); // Y acceleration
      AcZ = Wire.read()<<8|Wire.read(); // Z acceleration

      // Get magnitude
      magnitude = AcX + AcY + AcZ;

      // Check if baseline has been set
      if(baseline != 0) {

        // If the magnitude is outside of the range of baseline + or - sensitivity the bike is being moved
        if((magnitude > (baseline + sensitivity)) || (magnitude < (baseline - sensitivity))) {
          // Sound the alarm
          soundAlarm();
        } else {
          // Else add to the baseline measurement
          baseline = magnitude + baseline;
          baseline = baseline/2;
        }
      } else {
        // If baseline not set then set it
        baseline = magnitude;
      }

    // Delay before next read
    delay(100);

  } else {
    // Enter a low power mode
    //LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
  }
}
