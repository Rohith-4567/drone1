#include "AccessPoint.h"
#include "Calibrate.h"
#include "MPU.h"
#include "control_params.h"
#include <Arduino.h>
#include <ESP32Servo.h>

#include "rtos_app.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  controlParamsInit();  // Must run before any controlParamsGetCopy/controlParamsUpdate

  initESCs();
  delay(100);
  calibrateESCs();

  while (!initMPU()) {
    Serial.println("Failed to initialize MPU, retrying...");
    delay(1000);
  }

  setupAccessPoint();
  Serial.println("AP setup complete");

  // PID init (keep your tuning)
  Input = pitch_deg();
  Setpoint = 0;
  myPID.SetMode(myPID.Control::automatic);
  myPID.SetTunings(5, 0, .3);
  myPID.SetOutputLimits(-500, 500);

  // Start RTOS tasks + DRDY interrupt
  rtosStart();
}

void loop() {
  // Don’t do work here; RTOS tasks are running.
  vTaskDelay(pdMS_TO_TICKS(1000));
}