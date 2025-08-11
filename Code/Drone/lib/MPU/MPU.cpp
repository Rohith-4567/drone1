#include "MPU.h"
#include <math.h>

// --- MPU object: SPI, CS on pin 5 ---
bfs::Mpu6500 imu(&SPI, 5);

// Constants
constexpr float rad2deg = 180.0f / 3.14159265358979323846f;

// Yaw integration
static float yaw = 0.0f;          // radians
static unsigned long last_ms = 0; // ms

// Initialize MPU and SPI
bool initMPU()
{

    SPI.begin(18, 19, 23, 5); // SCLK, MISO, MOSI, CS

    if (!imu.Begin())
    {
        Serial.println("Error initializing communication with IMU");
        return false;
    }
    if (!imu.ConfigSrd(19))
    {
        Serial.println("Error configuring SRD");
        return false;
    }

    last_ms = millis();
    return true;
}

// Roll from accelerometer
float roll_deg()
{
    return atan2(imu.accel_y_mps2(), imu.accel_z_mps2()) * rad2deg;
}

// Pitch from accelerometer
float pitch_deg()
{
    float denom = sqrt(imu.accel_y_mps2() * imu.accel_y_mps2() +
                       imu.accel_z_mps2() * imu.accel_z_mps2());
    return atan2(-imu.accel_x_mps2(), denom) * rad2deg;
}

// Yaw (degrees)
float yaw_deg()
{
    return yaw * rad2deg;
}

// Update yaw integration
void updateYaw()
{
    if (imu.Read())
    {
        unsigned long now_ms = millis();
        float dt = (now_ms - last_ms) / 1000.0f;
        last_ms = now_ms;

        yaw += imu.gyro_z_radps() * dt;

        // Wrap yaw to -pi..pi
        if (yaw > M_PI)
            yaw -= 2.0f * M_PI;
        if (yaw < -M_PI)
            yaw += 2.0f * M_PI;
    }
}