#include "LinearActuator.h"
#include <ratio>

void debug(char *msg, ...);

LinearActuator::LinearActuator(int leg, int actuator, ControlerAB pins, int pos_min, int pos_max,
                               double freq)
    : leg(leg), actuator(actuator), posMinNano(pos_min), posMaxNano(pos_max),
      posNano(-1) {
  minTickDt = 1.0 / freq;
  pwm = new PwmOut(pins.PWM);
  pwm->period_us(PeriodUsController);
  pwm->write(0);
  dir1 = new DigitalOut(pins.DIR1);
  dir2 = new DigitalOut(pins.DIR2);

  dir1->write(0);
  dir2->write(0);
  lastTick = Kernel::Clock::now();
  position = -1;
  actuatorLength = 0.2;
  targetPosition = -1; // i.e. not set
  pidIValue = 0.0;
  pidIMax = 10.0;
  pidDValue = 0.0;
  targetPwm = 0.0;
  Kp =
      20.0; // error goes from -0.2 to 0.2, for 100 got 100% until 0.01 distance
  Ki = 0;
  Kd = 0;
  /*
    // dtsec is expected to be 1/Freq = minTickDt
    double err = targetPosition -
                 position; // value can be positive or negative, below 0.2
    pidIValue += err * dtsec;
    if (pidIValue>pidIMax) pidIValue=pidIMax;
    if (pidIValue<-pidIMax) pidIValue=-pidIMax;

    targetPwm = Kp * err + Ki * pidIValue + Kd * (pidDValue - err) / dtsec;
    pidDValue = err;
  */
  currentPwm = 0.0;
  slopePwm = 0.2; // 0 to 1.0 in 0.2sec
  maxPwm = 0.95;
  safeArea = 0.05; // 5cm from extremities
  safePwm = 0.5;   // limit power to 50%

  emergencyStopArea = 0.01; // 1cm from extremities
  lostPositionAfter = 0.1;  // if no update for position, stop
}

LinearActuator::~LinearActuator() {
  delete pwm;
  delete dir1;
  delete dir2;
}

double LinearActuator::getPosition() { return position; }
double LinearActuator::getTargetPosition() { return targetPosition; }
double LinearActuator::getTargetPwm() { return targetPwm; }
double LinearActuator::getCurrentPwm() { return currentPwm; }

void LinearActuator::stop() {
  targetPwm = 0;
  currentPwm = 0;
  targetPosition = position;
  set(0, 0, 0);
}

void LinearActuator::setPositionNano(uint16_t mesure_int) {
  mutex.lock();
  posNano = mesure_int;
  if (posNano < posMinNano)
    position = 0.0;
  else if (posNano > posMaxNano)
    position = actuatorLength;
  else
    position = actuatorLength * (double)(mesure_int - posMinNano) /
               (double)(posMaxNano - posMinNano);
  lastNanoRead = Kernel::Clock::now();
  mutex.unlock();
}

void LinearActuator::set(int dir1, int dir2, double pwm) {
  this->dir1->write(dir1);
  this->dir2->write(dir2);
  this->pwm->write(pwm);
  //  debug("set %d %d %lf\n", dir1, dir2, pwm);
}

void LinearActuator::moveTo(double position, double power) {
  if (power < 0)
    maxPwm = 0.95;
  else
    maxPwm = power;
  if (position > actuatorLength)
    position = actuatorLength;

  if ((position >= 0) && (position <= actuatorLength)) {
    targetPosition = position;
    if (targetPosition > this->position) {
      targetPwm = maxPwm;
    } else {
      targetPwm = -maxPwm;
    }
  }
}

void print(const char *msg, ...); // comes from main.c, used to debug on serial-stdout
void LinearActuator::tick() {
  mutex.lock();
  double position;
  position = this->position;
  // check nano last time read:
  auto ndt = Kernel::Clock::now() - lastNanoRead;
  mutex.unlock();
  if ((std::chrono::duration<double>(ndt).count() > lostPositionAfter) ||
      (position < 0)) {
// we possibily lost nano, emergency stop
#ifdef DEBUG12
    if ((leg == 1) && (actuator == 2))
      print("L12-- lost nano\n");
#endif
    currentPwm = 0.0;
    set(1, 1, 0.0);
    return;
  }

  auto dt = Kernel::Clock::now() - lastTick;
  double dtsec = std::chrono::duration<double>(dt).count();
  if (dtsec < minTickDt)
    return;
  lastTick = Kernel::Clock::now();
////
#ifdef DEBUG12
  if ((leg == 1) && (actuator == 2)) {
    print("L12#%.4lf#%.4lf#%.4lf#%.4lf#%lf#\n", position, targetPosition,
          currentPwm, targetPwm, dtsec);
  }
#endif

  if ((currentPwm == 0.0) && (targetPwm == 0.0)) {
    set(1, 1, 0.0);
    return;
  }
  // BEGIN
  // replace below by a PID:
  // targetPwm = Kp * abs(targetPosition-position)
  //            + Ki * d abs(targetPosition - position) / dt
  // if (abs(position - targetPosition) < 0.1) {
  // actuator arrived at destination...
  double err =
      targetPosition - position; // value can be positive or negative, below 0.2
  pidIValue += err * dtsec;
  if (pidIValue > pidIMax)
    pidIValue = pidIMax;
  if (pidIValue < -pidIMax)
    pidIValue = -pidIMax;

  targetPwm = Kp * err + Ki * pidIValue + Kd * (pidDValue - err) / dtsec;
  pidDValue = err;
#ifdef DEBUG12
  if ((leg == 1) && (actuator == 2))
    print("L12--err: %lf (c:%lf / t:%lf)\n", err, currentPwm, targetPwm);
#endif
  //}
  // END
  if (targetPwm > maxPwm)
    targetPwm = maxPwm;
  if (targetPwm < -maxPwm)
    targetPwm = -maxPwm;

  // check on position
  //  - emergency
  if (((position < emergencyStopArea) && (targetPwm < 0)) ||
      ((position > (actuatorLength - emergencyStopArea)) && (targetPwm > 0))) {
#ifdef DEBUG12
    if ((leg == 1) && (actuator == 2))
      print("L12--emergency\n");
#endif
    currentPwm = 0.0;
    set(1, 1, 0.0);
    return;
  }
  // - safe
  if ((position < safeArea) && (targetPwm < 0)) {
#ifdef DEBUG12
    if ((leg == 1) && (actuator == 2))
      print("L12--safe-\n");
#endif
    targetPwm = std::max(targetPwm, -safePwm);
  }
  if ((position > (actuatorLength - safeArea)) && (targetPwm > 0)) {
#ifdef DEBUG12
    if ((leg == 1) && (actuator == 2))
      print("L12--safe+\n");
#endif
    targetPwm = std::min(targetPwm, safePwm);
  }

  // adjust current pwm according to target and slope
  if (currentPwm > targetPwm) {
    currentPwm = currentPwm - dtsec / slopePwm;
    if (currentPwm < targetPwm)
      currentPwm = targetPwm;
  } else {
    currentPwm = currentPwm + dtsec / slopePwm;
    if (currentPwm > targetPwm)
      currentPwm = targetPwm;
  }

#ifdef DEBUG12
  if ((leg == 1) && (actuator == 2))
    print("L12-- set (c:%lf / t:%lf)\n", currentPwm, targetPwm);
#endif

  if (currentPwm > 0)
    set(0, 1, currentPwm);
  else
    set(1, 0, abs(currentPwm));
}

/*
void LinearActuator::setSens(LinearActuator::dir_t sensTarget) {
  switch (sensTarget) {
  // FIXME sens de la commande
  case forward:
    dir1->write(1);
    dir2->write(0);
    break;
  case backward:
    dir1->write(0);
    dir2->write(1);
    break;
  case none:
    dir1->write(0);
    dir2->write(0);
    break;
  }
}

void LinearActuator::setPwm(float intensity) {
  float cmd;
  if (intensity > 1) {
    cmd = 1;
  } else if (intensity < 0) {
    cmd = 0;
  } else {
    cmd = intensity;
  }
  pwm->write(cmd);
}

void LinearActuator::setPositionInt(uint16_t mesure_int) {
  positionInt = mesure_int;
  positionMm = resolution * (positionInt - trigMin);
}
*/
