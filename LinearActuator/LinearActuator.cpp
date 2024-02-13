#include "LinearActuator.h"
#include <ratio>

void debug(char *msg, ...);

LinearActuator::LinearActuator(int leg, int actuator, ControlerAB pins,
                               int pos_min, int pos_max, double freq)
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
  targetPwm = 0.0;

  minPwm = 0.1;
  Kp = 50.0; // error goes is in m, with Kp=50 we have 100% PWM until 0.02m
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
  slopePwm = 0.02; // 0 to 1.0 in 20ms
  maxPwm = 0.98;
  safeArea = 0.03; // 5cm from extremities
  safePwm = 0.5;   // limit power to 50%

  emergencyStopArea = 0.01; // 1cm from extremities
  lostPositionAfter = 0.2;  // if no update for position, stop
}

LinearActuator::~LinearActuator() {
  delete pwm;
  delete dir1;
  delete dir2;
}

double LinearActuator::getPosition() {
  double p;
  p = position;
  return p;
}
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
  posNano = mesure_int;
  if (posNano < posMinNano)
    position = 0.0;
  else if (posNano > posMaxNano)
    position = actuatorLength;
  else
    position = actuatorLength * (double)(mesure_int - posMinNano) /
               (double)(posMaxNano - posMinNano);
  lastNanoRead = Kernel::Clock::now();
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
  else {
    if (power > 0.98)
      power = 0.98;
    maxPwm = power;
  }
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

void print(const char *msg,
           ...); // comes from main.c, used to debug on serial-stdout

void LinearActuator::tick() {
  status = LA_STATUS_OK;
  double position;
  position = this->position;
  // check nano last time read:
  auto ndt = Kernel::Clock::now() - lastNanoRead;
  if ((std::chrono::duration<double>(ndt).count() > lostPositionAfter) ||
      (position < 0)) {
    status = LA_STATUS_MISSING_NANO;
    // we possibily lost nano, emergency stop
    // debug("lost nano!!");
    currentPwm = 0.0;
    position = -1.0;
    set(1, 1, 0.0);
    return;
  }

  auto dt = Kernel::Clock::now() - lastTick;
  double dtsec = std::chrono::duration<double>(dt).count();
  if (dtsec < minTickDt)
    return;
  nticking += 1;
  lastTick = Kernel::Clock::now();

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
  if (fabs(err) < 0.001)         // reach destination
    targetPwm = 0.0;             // stop actuator
  else {
    targetPwm = Kp * err;
    if (fabs(targetPwm) < minPwm) {
      if (targetPwm < 0)
        targetPwm = -minPwm;
      else
        targetPwm = minPwm;
    }
  }
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
    status = LA_STATUS_EMERGENCY_AREA;
    currentPwm = 0.0;
    set(1, 1, 0.0);
    return;
  }
  // - safe
  if ((position < safeArea) && (targetPwm < 0)) {
    status = LA_STATUS_SAFE_AREA;
    targetPwm = std::max(targetPwm, -safePwm);
  }
  if ((position > (actuatorLength - safeArea)) && (targetPwm > 0)) {
    status = LA_STATUS_SAFE_AREA;
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

  // apply pwm
  if (currentPwm > 0)
    set(0, 1, currentPwm);
  else
    set(1, 0, abs(currentPwm));
}
