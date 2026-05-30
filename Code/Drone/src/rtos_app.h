#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "QuickPID.h"
#include <Arduino.h>

void rtosStart();

// PID state used by main.cpp for init and by control task
extern float Input, Setpoint, Output;
extern QuickPID myPID;

// Expose handles if you want
extern TaskHandle_t gControlTaskHandle;
extern TaskHandle_t gWifiTaskHandle;