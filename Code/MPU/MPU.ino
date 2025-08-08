/* 
 * Brian R Taylor
 * brian.taylor@bolderflight.com
 *
 * Copyright (c) 2021 Bolder Flight Systems Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mpu6500.h"
#include <math.h>          // atan2(), sqrt()

// Mpu6500 object, SPI bus, CS on pin 10 (example – adjust as needed)
bfs::Mpu6500 imu(&SPI, 5);

// ---------- Angle helpers ----------
constexpr float rad2deg = 180.0f / 3.14159265358979323846f;

// Roll from accelerometer (rad -> deg)
float roll_deg()
{
    // atan2(y, z)
    return atan2(imu.accel_y_mps2(), imu.accel_z_mps2()) * rad2deg;
}

// Pitch from accelerometer (rad -> deg)
float pitch_deg()
{
    // atan2(-x, sqrt(y^2 + z^2))
    float denom = sqrt(imu.accel_y_mps2() * imu.accel_y_mps2() +
                       imu.accel_z_mps2() * imu.accel_z_mps2());
    return atan2(-imu.accel_x_mps2(), denom) * rad2deg;
}

// Yaw (rad -> deg) – integrated from gyro‑z
static float yaw = 0.0f;            // rad
static unsigned long last_ms = 0;   // ms

float yaw_deg()
{
    return yaw * rad2deg;
}

// ---------- Setup ----------
void setup()
{
    /* Serial to display data */
    Serial.begin(115200);
    while (!Serial) {}

    /* Start the SPI bus */
    SPI.begin(18, 19, 23, 5); // SCLK, MISO, MOSI, CS

    /* Initialize and configure IMU */
    if (!imu.Begin()) {
        Serial.println("Error initializing communication with IMU");
        while (1) {}
    }

    /* Set the sample rate divider */
    if (!imu.ConfigSrd(19)) {
        Serial.println("Error configured SRD");
        while (1) {}
    }

    /* Initialise timing */
    last_ms = millis();
}

// ---------- Loop ----------
void loop()
{
    /* Check if data read */
    if (imu.Read()) {
        // --- Update yaw from gyro z (rad/s) ---
        unsigned long now_ms = millis();
        float dt = (now_ms - last_ms) / 1000.0f;      // seconds
        last_ms = now_ms;
        yaw += imu.gyro_z_radps() * dt;              // integrate
        // Wrap yaw to -pi..pi for stability (optional)
        if (yaw > M_PI) yaw -= 2.0f * M_PI;
        if (yaw < -M_PI) yaw += 2.0f * M_PI;

        // --- Print data ---
        Serial.print(imu.new_imu_data());    // optional: frame counter / timestamp
        Serial.print('\t');

        // Roll, Pitch, Yaw (degrees)
        Serial.print("Roll : ");
        Serial.print(roll_deg());
        Serial.print(" ");
        Serial.print("Pitch : ");
        Serial.print(pitch_deg());
        Serial.print(" ");
        Serial.print("Yaw : ");
        Serial.print(yaw_deg());
        Serial.print(" ");

        // Optional raw data (uncomment if you still want it)
        //Serial.print('\t');
        //Serial.print(imu.accel_x_mps2());
        //Serial.print('\t');
        //Serial.print(imu.accel_y_mps2());
        //Serial.print('\t');
        //Serial.print(imu.accel_z_mps2());
        //Serial.print('\t');
        //Serial.print(imu.gyro_x_radps());
        //Serial.print('\t');
        //Serial.print(imu.gyro_y_radps());
        //Serial.print('\t');
        //Serial.print(imu.gyro_z_radps());

        Serial.print(imu.die_temp_c());

        Serial.println();     // newline
        delay(100);
    }
}
