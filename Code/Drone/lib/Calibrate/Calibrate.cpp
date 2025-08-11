#include "Calibrate.h"
#include <ESP32Servo.h>

#define ESC_PIN1 12
#define ESC_PIN2 14
#define ESC_PIN3 26
#define ESC_PIN4 27

Servo esc1, esc2, esc3, esc4;

void initESCs()
{
  esc1.attach(ESC_PIN1, 1000, 2000); // min and max pulse width (in microseconds)
  esc2.attach(ESC_PIN2, 1000, 2000); // min and max pulse width (in microseconds)
  esc3.attach(ESC_PIN3, 1000, 2000); // min and max pulse width (in microseconds)
  esc4.attach(ESC_PIN4, 1000, 2000); // min and max pulse width (in microseconds)
  delay(5000);
}

void calibrateESCs()
{

  // Serial.println("Type '1' after powering on the ESC to start calibration.");
  // while (!Serial.available())
  //   ;            // Wait for user input
  // Serial.read(); // Clear the input buffer

  Serial.println("Sending MAX signal to calibrate...");
  esc1.writeMicroseconds(2000); // Max throttle
  esc2.writeMicroseconds(2000); // Max throttle
  esc3.writeMicroseconds(2000); // Max throttle
  esc4.writeMicroseconds(2000); // Max throttle
  delay(2000);

  Serial.println("Now sending MIN signal...");
  esc1.writeMicroseconds(1000); // Min throttle
  esc2.writeMicroseconds(1000); // Min throttle
  esc3.writeMicroseconds(1000); // Min throttle
  esc4.writeMicroseconds(1000); // Min throttle
  delay(2000);

  Serial.println("ESC should now be calibrated.");
  delay(10000);
}

void setThrottle(int speed)
{
  speed = constrain(speed, 1000, 2000);
  esc1.writeMicroseconds(speed);
  esc2.writeMicroseconds(speed);
  esc3.writeMicroseconds(speed);
  esc4.writeMicroseconds(speed);

  Serial.print("Throttle set to: ");
  Serial.println(speed);
}

void frontLeft(int speed)
{
  esc1.writeMicroseconds(speed);
}

void frontRight(int speed)
{
  esc2.writeMicroseconds(speed);
}

void backLeft(int speed)
{
  esc3.writeMicroseconds(speed);
}

void backRight(int speed)
{
  esc4.writeMicroseconds(speed);
}