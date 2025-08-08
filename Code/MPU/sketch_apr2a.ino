#include <Arduino.h>
#include <Wire.h>
#include "mpu6500.h"

// Sensor object
bfs::Mpu6500 imu;

// Timing
unsigned long prev_time = 0;
float dt = 0.0;

// Orientation
float roll = 0.0, pitch = 0.0, yaw = 0.0;

// Complementary filter constant
const float alpha = 0.98;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Wire.begin();  // I2C pins for ESP32
  Wire.setClock(400000); // Fast I2C

  imu.Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);

  if (!imu.Begin()) {
    Serial.println("IMU init failed");
    while (1);
  }

  if (!imu.ConfigSrd(19)) { // ~50 Hz
    Serial.println("SRD config failed");
    while (1);
  }

  prev_time = millis();
}

void loop() {
  if (imu.Read()) {
    unsigned long curr_time = millis();
    dt = (curr_time - prev_time) / 1000.0; // in seconds
    prev_time = curr_time;

    // Get raw accelerometer and gyro values
    float ax = imu.accel_x_mps2();
    float ay = imu.accel_y_mps2();
    float az = imu.accel_z_mps2();
    float gx = imu.gyro_x_radps();
    float gy = imu.gyro_y_radps();
    float gz = imu.gyro_z_radps();

    // Convert gyro rad/s to deg/s
    gx *= 180.0 / PI;
    gy *= 180.0 / PI;
    gz *= 180.0 / PI;

    // Calculate roll and pitch from accelerometer (degrees)
    float accel_roll = atan2(ay, az) * 180.0 / PI;
    float accel_pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

    // Integrate gyro for roll and pitch
    roll = alpha * (roll + gx * dt) + (1 - alpha) * accel_roll;
    pitch = alpha * (pitch + gy * dt) + (1 - alpha) * accel_pitch;

    // Yaw from integrated gyro only (will drift)
    yaw += gz * dt;

    // Print angles
    Serial.print("Roll: "); Serial.print(roll);
    Serial.print("\tPitch: "); Serial.print(pitch);
    Serial.print("\tYaw: "); Serial.println(yaw);
  }
}