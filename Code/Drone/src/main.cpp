#include <ESP32Servo.h>
#include "Calibrate.h"
#include "MPU.h"
#include "AccessPoint.h"
#include "QuickPID.h"

int PWMthrottle = 1000;

float Setpoint, Input, Output;
QuickPID myPID(&Input, &Output, &Setpoint);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    initESCs();
    delay(100);
    calibrateESCs();

    while (!initMPU())
    {
        Serial.println("Failed to initialize MPU, retrying...");
        delay(1000);
    }
    setupAccessPoint();
    Serial.println("AP setup complete");

    Input = pitch_deg();
    Setpoint = 0; // Target pitch angle
    myPID.SetMode(myPID.Control::automatic);
    myPID.SetTunings(5, 0, 0);
    myPID.SetOutputLimits(-500, 500); // Allow output to go negative and positive
}

void loop()
{
    updateYaw();

    accessPointLoop();
    PWMthrottle = map(throttle, 0, 100, 1000, 2000); // Map throttle from 0-100 to 1000-2000 PWM range
    setThrottle(PWMthrottle);
    Input = pitch_deg();
    myPID.Compute();
    frontLeft(PWMthrottle + Output);
    frontRight(PWMthrottle + Output);
    backLeft(PWMthrottle - Output);
    backRight(PWMthrottle - Output);

    Serial.print(pitch_deg());
    Serial.print(" ");
    Serial.println(Output);

    delay(100);
}