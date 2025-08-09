

#ifndef ACCESS_POINT_H
#define ACCESS_POINT_H

#include <WiFi.h>
#include <WebServer.h>

extern WebServer server;

extern float Kp_roll, Ki_roll, Kd_roll;
extern float Kp_pitch, Ki_pitch, Kd_pitch;
extern float throttle;

void setupAccessPoint();
void accessPointLoop();

#endif // ACCESS_POINT_H