#include <ESP32Servo.h>
#include "Calibrate.h"
#include "MPU.h"
#include "AccessPoint.h"

void setup()
{
    Serial.begin(115200);
    delay(1000);
    initESCs();
    delay(100);
    calibrateESCs();

    // while (!initMPU())
    // {
    //     Serial.println("Failed to initialize MPU, retrying...");
    //     delay(1000);
    // }
    setupAccessPoint();
    Serial.println("AP setup complete");
}

void loop()
{
    // for (int i = 1000; i <= 2000; i += 10)
    // {
    //     frontLeft(i);
    //     backLeft(i);
    //     frontRight(3000 - i);
    //     backRight(3000 - i);
    //     Serial.println(i);
    //     delay(100);
    // }
    // for (int i = 2000; i >= 1000; i -= 10)
    // {
    //     frontLeft(i);
    //     backLeft(i);
    //     frontRight(3000 - i);
    //     backRight(3000 - i);
    //     Serial.println(i);
    //     delay(100);
    // }
    accessPointLoop();
    Serial.printf("Current PID values: Kp_roll=%.3f, Ki_roll=%.3f, Kd_roll=%.3f\n",
                  Kp_roll, Ki_roll, Kd_roll);
    Serial.printf("Current PID values: Kp_pitch=%.3f, Ki_pitch=%.3f, Kd_pitch=%.3f\n",
                  Kp_pitch, Ki_pitch, Kd_pitch);
    delay(1000);
}