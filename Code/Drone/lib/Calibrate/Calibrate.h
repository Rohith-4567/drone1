#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <ESP32Servo.h>

// Function declarations
void initESCs();
void calibrateESCs();
void setThrottle(int speed);
void frontLeft(int speed);
void frontRight(int speed);
void backLeft(int speed);
void backRight(int speed);

#endif