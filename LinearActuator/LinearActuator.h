#ifndef MEGABOT_DRIVER_LINEARACTUATOR_H
#define MEGABOT_DRIVER_LINEARACTUATOR_H

#include "GlobalConfig.h"
#include "mbed.h"

struct ControlerAB{
    PinName DIR1;
    PinName DIR2;
    PinName PWM;
    ControlerAB(PinName DIR1,PinName DIR2,PinName PWM):DIR1(DIR1),DIR2(DIR2),PWM(PWM){}
};

#define LA_STATUS_OK               1
#define LA_STATUS_SAFE_AREA        2
#define LA_STATUS_EMERGENCY_AREA   4
#define LA_STATUS_MISSING_NANO     8


class LinearActuator {
public:
  LinearActuator(int leg, int actuator, ControlerAB pins,
                 int pos_min, int pos_max, double freq);

  ~LinearActuator();

  double getPosition();
  double getTargetPosition();
  double getTargetPwm();
  double getCurrentPwm();

  void setPositionNano(uint16_t mesure_int);
  
  void stop();

  void moveTo(double position, double power);

  void tick();

  int leg, actuator;

  unsigned char status;
private:
  Mutex mutex;
  // id
  // position
  int posMinNano, posMaxNano, posNano;
  double position;
  double actuatorLength;
  // target
  double targetPosition;
  double Kp;
  // pwm
  double targetPwm;
  double currentPwm;
  double slopePwm;
  double maxPwm;
  double minPwm;
  // safe area:
  double safeArea;
  double safePwm;
  // emergy:
  double emergencyStopArea;
  double lostPositionAfter;

  int dbg_cpt = 0;

  rtos::Kernel::Clock::time_point lastNanoRead;
  rtos::Kernel::Clock::time_point lastTick;
  double minTickDt;
  void set(int dir1, int dir2, double pwm);
  PwmOut *pwm;
  DigitalOut *dir1;
  DigitalOut *dir2;
};

#endif // MEGABOT_DRIVER_LINEARACTUATOR_H
