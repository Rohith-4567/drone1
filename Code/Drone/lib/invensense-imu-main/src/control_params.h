#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <Arduino.h>

struct ControlParams {
  float Kp_roll, Ki_roll, Kd_roll;
  float Kp_pitch, Ki_pitch, Kd_pitch;
  float throttle; // 0..100
};

void controlParamsInit();
ControlParams controlParamsGetCopy();
void controlParamsSetFromWeb(const ControlParams &newVals);

// Optional helper: update only fields that changed
void controlParamsUpdate(bool setKpRoll, float vKpRoll, bool setKiRoll,
                         float vKiRoll, bool setKdRoll, float vKdRoll,
                         bool setKpPitch, float vKpPitch, bool setKiPitch,
                         float vKiPitch, bool setKdPitch, float vKdPitch,
                         bool setThrottle, float vThrottle);