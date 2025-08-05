#include <ESP32Servo.h>

#define ESC_PIN 12  // Connect ESC signal wire to D9

Servo esc;

void setup() {
  Serial.begin(9600);
  delay(1000);
  esc.attach(ESC_PIN, 1000, 2000);  // min and max pulse width (in microseconds)
  
  Serial.println("Type '1' after powering on the ESC to start calibration.");
  while (!Serial.available());  // Wait for user input
  Serial.read(); // Clear the input buffer

  Serial.println("Sending MAX signal to calibrate...");
  esc.writeMicroseconds(2000);  // Max throttle
  delay(2000);

  Serial.println("Now sending MIN signal...");
  esc.writeMicroseconds(1000);  // Min throttle
  delay(2000);

  Serial.println("ESC should now be calibrated.");
}

void loop() {
  int speed;
  Serial.println("Enter throttle value (1000 to 2000):");
  
  while (Serial.available() == 0);
  speed = Serial.parseInt();
  speed = constrain(speed, 1000, 2000);
  
  esc.writeMicroseconds(speed);
  Serial.print("Throttle set to: ");
  Serial.println(speed);

  delay(1000);  // Small delay before reading next input
}