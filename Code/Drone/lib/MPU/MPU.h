#ifndef MPU_H
#define MPU_H

#include <Arduino.h>
#include <SPI.h>
#include "mpu6500.h"

// Initialize MPU (call in setup)
bool initMPU();

// Read angles (degrees)
float roll_deg();
float pitch_deg();
float yaw_deg();

// Update yaw integration (call every loop)
void updateYaw();

#endif