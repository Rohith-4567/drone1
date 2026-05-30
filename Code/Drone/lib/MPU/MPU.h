#ifndef MPU_H
#define MPU_H

#include "mpu6500.h"
#include <Arduino.h>
#include <SPI.h>

// Initialize MPU (call in setup)
bool initMPU();

// Read angles (degrees)
float roll_deg();
float pitch_deg();
float yaw_deg();
bool imuUpdate();

#endif