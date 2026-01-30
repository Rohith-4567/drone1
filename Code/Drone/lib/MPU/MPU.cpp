#include "MPU.h"
#include <math.h>

// --- MPU object: SPI, CS on pin 5 ---
bfs::Mpu6500 imu(&SPI, 5);

constexpr float rad2deg = 180.0f / 3.14159265358979323846f;

static float yaw = 0.0f;                 // radians
static unsigned long last_ms = 0;        // ms

// Cached derived angles (degrees) from last imu.Read()
static float roll_cache_deg  = 0.0f;
static float pitch_cache_deg = 0.0f;

bool initMPU() {
    SPI.begin(18, 19, 23, 5);

    if (!imu.Begin()) {
        Serial.println("Error initializing communication with IMU");
        return false;
    }

    if (!imu.EnableDrdyInt()) {
        Serial.println("Error enabling DRDY interrupt");
        return false;
    }

    if (!imu.ConfigSrd(19)) {
        Serial.println("Error configuring SRD");
        return false;
    }

    last_ms = millis();
    return true;
}

bool imuUpdate() {
    if (!imu.Read()) return false;


    unsigned long now_ms = millis();
    float dt = (now_ms - last_ms) / 1000.0f;
    last_ms = now_ms;

    // Integrate yaw from gyro z
    yaw += imu.gyro_z_radps() * dt;

    // Wrap yaw to -pi..pi
    if (yaw > M_PI)  yaw -= 2.0f * M_PI;
    if (yaw < -M_PI) yaw += 2.0f * M_PI;

    // Compute roll/pitch from this same accel sample
    roll_cache_deg = atan2(imu.accel_y_mps2(), imu.accel_z_mps2()) * rad2deg;

    float denom = sqrt(imu.accel_y_mps2()*imu.accel_y_mps2() +
                       imu.accel_z_mps2()*imu.accel_z_mps2());
    pitch_cache_deg = atan2(-imu.accel_x_mps2(), denom) * rad2deg;

    return true;
}

float roll_deg()  { return roll_cache_deg; }
float pitch_deg() { return pitch_cache_deg; }
float yaw_deg()   { return yaw * rad2deg; }