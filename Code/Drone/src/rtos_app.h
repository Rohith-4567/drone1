#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <Arduino.h>

void rtosStart();

// Expose handles if you want
extern TaskHandle_t gControlTaskHandle;
extern TaskHandle_t gWifiTaskHandle;