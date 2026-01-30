#include "control_params.h"

static SemaphoreHandle_t gParamsMutex = nullptr;
static ControlParams gParams;

void controlParamsInit() {
  gParamsMutex = xSemaphoreCreateMutex();

  // Defaults (match your current globals)
  gParams.Kp_roll = 1.0f;
  gParams.Ki_roll = 0.0f;
  gParams.Kd_roll = 0.0f;
  gParams.Kp_pitch = 1.0f;
  gParams.Ki_pitch = 0.0f;
  gParams.Kd_pitch = 0.0f;
  gParams.throttle = 0.0f;
}

ControlParams controlParamsGetCopy() {
  ControlParams copy;
  xSemaphoreTake(gParamsMutex, portMAX_DELAY);
  copy = gParams;
  xSemaphoreGive(gParamsMutex);
  return copy;
}

void controlParamsSetFromWeb(const ControlParams &newVals) {
  xSemaphoreTake(gParamsMutex, portMAX_DELAY);
  gParams = newVals;
  xSemaphoreGive(gParamsMutex);
}

void controlParamsUpdate(bool setKpRoll, float vKpRoll, bool setKiRoll,
                         float vKiRoll, bool setKdRoll, float vKdRoll,
                         bool setKpPitch, float vKpPitch, bool setKiPitch,
                         float vKiPitch, bool setKdPitch, float vKdPitch,
                         bool setThrottle, float vThrottle) {
  xSemaphoreTake(gParamsMutex, portMAX_DELAY);
  if (setKpRoll)
    gParams.Kp_roll = vKpRoll;
  if (setKiRoll)
    gParams.Ki_roll = vKiRoll;
  if (setKdRoll)
    gParams.Kd_roll = vKdRoll;
  if (setKpPitch)
    gParams.Kp_pitch = vKpPitch;
  if (setKiPitch)
    gParams.Ki_pitch = vKiPitch;
  if (setKdPitch)
    gParams.Kd_pitch = vKdPitch;
  if (setThrottle)
    gParams.throttle = vThrottle;
  xSemaphoreGive(gParamsMutex);
}