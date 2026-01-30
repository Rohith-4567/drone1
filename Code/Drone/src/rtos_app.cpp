#include "rtos_app.h"

#include "AccessPoint.h"
#include "Calibrate.h"
#include "MPU.h"
#include "QuickPID.h"
#include "control_params.h"
#include <ESP32Servo.h>

// -------------------- Config --------------------
static constexpr int CORE_CONTROL = 1; // pin PID + motors here
static constexpr int CORE_WIFI = 0;    // WiFi/AP + other noise here

static constexpr gpio_num_t MPU_DRDY_GPIO = GPIO_NUM_25; // CHANGE to your pin

// -------------------- Control objects --------------------
static int PWMthrottle = 1000;

static float Setpoint = 0.0f, Input = 0.0f, Output = 0.0f;
static QuickPID myPID(&Input, &Output, &Setpoint);

static volatile bool gOff = false;

// Task handles
TaskHandle_t gControlTaskHandle = nullptr;
TaskHandle_t gWifiTaskHandle = nullptr;

// -------------------- ISR: DRDY -> notify control task --------------------
static void IRAM_ATTR mpuDrdyIsr() {
  BaseType_t higherWoken = pdFALSE;
  vTaskNotifyGiveFromISR(gControlTaskHandle, &higherWoken);
  if (higherWoken)
    portYIELD_FROM_ISR();
}

// -------------------- Tasks --------------------
static void controlTask(void *arg) {

  // Optional: update tunings at a lower rate than the control loop
  uint32_t tickCount = 0;

  // Wait for first DRDY before starting outputs
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // blocks until DRDY ISR notifies

    // If we've faulted out, keep motors at minimum and ignore further updates
    if (gOff) {
      setThrottle(1000);
      continue;
    }

    // Grab latest throttle + gains from the WiFi task safely (mutex-protected)
    ControlParams p = controlParamsGetCopy();

    // Compute throttle every tick (fast path)
    PWMthrottle = map((int)p.throttle, 0, 100, 1000, 2000);

    // Update PID tunings at ~50 Hz if your loop is ~1 kHz (adjust divisor if
    // needed)
    tickCount++;
    if ((tickCount % 20) == 0) {
      myPID.SetTunings(p.Kp_pitch, p.Ki_pitch, p.Kd_pitch);
    }

    // Read a coherent IMU sample for this control tick
    if (!imuUpdate()) {
      // If no new sample, skip this tick rather than reusing stale Output
      continue;
    }

    // Control
    Input = pitch_deg();
    myPID.Compute();

    // Apply base throttle then motor mix
    setThrottle(PWMthrottle);
    frontLeft(PWMthrottle + Output);
    frontRight(PWMthrottle + Output);
    backLeft(PWMthrottle - Output);
    backRight(PWMthrottle - Output);

    // Safety cutoff
    float pdeg = pitch_deg();
    if (pdeg > 80.0f || pdeg < -80.0f) {
      setThrottle(1000);
      gOff = true;
    }
  }
}

static void wifiTask(void *arg) {
  // Runs slower, does not need to be tight
  TickType_t lastWake = xTaskGetTickCount();

  for (;;) {
    accessPointLoop();

    // Throttle prints so they don't spam and jitter everything
    static uint32_t lastPrintMs = 0;
    uint32_t now = millis();
    if (now - lastPrintMs > 50) { // 20 Hz logging
      lastPrintMs = now;
      Serial.print(pitch_deg());
      Serial.print(" ");
      Serial.println(Output);
    }

    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(5));
  }
}

// -------------------- Public start --------------------
void rtosStart() {
  // Setup DRDY pin interrupt
  pinMode((int)MPU_DRDY_GPIO, INPUT);

  xTaskCreatePinnedToCore(controlTask, "controlTask", 4096, nullptr,
                          5, // higher priority
                          &gControlTaskHandle, CORE_CONTROL);

  xTaskCreatePinnedToCore(wifiTask, "wifiTask", 4096, nullptr,
                          2, // lower priority
                          &gWifiTaskHandle, CORE_WIFI);

  // assign tasks before attaching interrupt to avoid race condition
  attachInterrupt((int)MPU_DRDY_GPIO, mpuDrdyIsr, RISING);

  // Create tasks
}