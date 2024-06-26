#ifndef MEGABOT_DRIVER_LINEARACTUATOR_H
#define MEGABOT_DRIVER_LINEARACTUATOR_H

#include "Controler.h"
#include <Arduino.h>
#include "config.h"

#define LA_STATUS_OK 1
#define LA_STATUS_SAFE_AREA 2
#define LA_STATUS_EMERGENCY_AREA 4
#define LA_STATUS_MISSING_NANO 8
#define LA_STATUS_STOP 0x10
#define LA_STATUS_MISSING 0x80


class LinearActuator {
public:
  LinearActuator(int leg, int actuator, ControlerAB pins, int pos_min,
                 int pos_max, float freq);

  ~LinearActuator();

  float getPosition();
  float getTargetPosition();
  float getTargetPwm();
  float getCurrentPwm();

  void setPositionNano(uint16_t mesure_int);

  void stop();

  void moveTo(float position, float power);
  void paramPID(float Kp, float Ki, float Kd);

  void tick();

  int leg, actuator;

  unsigned char status=LA_STATUS_MISSING;
  uint16_t posNanoRaw=0;
  unsigned int nticking = 0;
  unsigned int tickCalls=0;

public:
  // id
  // position
  int posMinNano, posMaxNano, posNano;
  float position = -1;
  float actuatorLength = 0.2;
  // target
  float targetPosition = -1;
  float Kp;
  float Ki;
  float Kd;
  float positionError=0;
  float cumulativeError=0;
  float maxCumulativeError;
  float lastError=0;
  int direction=0;
  // pwm
  float targetPwm=0.0;
  float currentPwm=0.0;
  float slopePwm=0.03; // 0 to 1.0 in 40ms
  float maxPwm=0.97;
  // safe area:
  float safeArea = 0.03; // 3cm from limits
  float safePwm = 0.5; // 50%
  // emergy:
  float emergencyStopArea = 0.01;
  uint32_t lostPositionAfterMs = 200; // in sec

  int dbg_cpt = 0;

  uint32_t lastNanoReadMs=0;
  uint32_t lastTickMs=0;
  uint32_t minTickDtMs=10;
  void set(int dir1, int dir2, float pwm);
  ControlerAB pins;
};

#endif // MEGABOT_DRIVER_LINEARACTUATOR_H
