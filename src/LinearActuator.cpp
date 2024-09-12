#include "LinearActuator.h"
#include <ratio>

void debug(char *msg, ...);

void print(const char *msg,
           ...); // comes from main.c, used to debug on serial-stdout

LinearActuator::LinearActuator(int leg, int actuator, ControlerAB pins,
                               int pos_min, int pos_max, float freq)
    : leg(leg), actuator(actuator), posMinNano(pos_min), posMaxNano(pos_max),
      posNano(-1), pins(pins)
{
  minTickDtMs = 1000.0 / freq;

  //  pwm = new PwmOut(pins.PWM);
  //  pwm->period_us(PeriodUsController);
  //  pwm->write(0);
  analogWriteFrequency(pins.PWM, 25000);
  pinMode(pins.PWM, OUTPUT);
  analogWrite(pins.PWM, 0);
  pinMode(pins.DIR1, OUTPUT);
  pinMode(pins.DIR2, OUTPUT);

  digitalWrite(pins.DIR1, LOW);
  digitalWrite(pins.DIR2, LOW);
  lastTickMs = millis();

  Kp = 20.0; // error is in m, max error in 0.2, with Kp=5 we have 100% PWM on max error
  // Kp=50 means pwm is 1.0 until 0.02 error
  Ki = 20.0;
  Kd = 0;
  positionError = 0;
  lastError = 0;
  cumulativeError = 0;
  maxCumulativeError = 1.0 / Ki; // don't cumulate if Ki leads to 100%
  /*
    // dtsec is expected to be 1/Freq = minTickDt
    float positionError = targetPosition -
                 position; // value can be positive or negative, below 0.2
    pidIValue += positionError * dtsec;
    if (pidIValue>pidIMax) pidIValue=pidIMax;
    if (pidIValue<-pidIMax) pidIValue=-pidIMax;

    targetPwm = Kp * positionError + Ki * pidIValue + Kd * (pidDValue - positionError) / dtsec;
    pidDValue = positionError;
  */
}

LinearActuator::~LinearActuator()
{
}

float LinearActuator::getPosition()
{
  return position;
}
float LinearActuator::getTargetPosition() { return targetPosition; }
float LinearActuator::getTargetPwm() { return targetPwm; }
float LinearActuator::getCurrentPwm() { return currentPwm; }

void LinearActuator::stop()
{
  targetPwm = 0;
  currentPwm = 0;
  targetPosition = position;
  set(0, 0, 0);
}

void LinearActuator::setPositionNano(uint16_t mesure_int)
{
  posNanoRaw = mesure_int;
  posNano = mesure_int;
  if (posNano < posMinNano)
    position = 0.0;
  else if (posNano > posMaxNano)
    position = actuatorLength;
  else
    position = actuatorLength * (float)(mesure_int - posMinNano) /
               (float)(posMaxNano - posMinNano);
  lastNanoReadMs = millis();

  //if (leg == 1 && actuator ==&)
  //Serial.printf("position: %f at %d \n", position, lastNanoReadMs);
}

void LinearActuator::paramPID(float Kp, float Ki, float Kd)
{
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  cumulativeError = 0;           // Reset integral ?
  maxCumulativeError = 1.0 / Ki; // update
}

void LinearActuator::set(int dir1, int dir2, float pwm)
{

  digitalWrite(pins.DIR1, dir1);
  digitalWrite(pins.DIR2, dir2);
  float v = pwm * (float)HW_PWM_MAX_VALUE;
  analogWrite(pins.PWM, v);
}

void LinearActuator::moveTo(float position, float power)
{
  if ((power < 0) || (power > 0.97))
    maxPwm = 0.97;
  else
    maxPwm=power;

  if (position > actuatorLength)
    position = actuatorLength;
  if (position < 0)
    position = 0;

  int previousDirection = direction;
  if (targetPosition > position)
    direction = 1; // extend
  else
    direction = -1; // retract

  targetPosition = position;
  if (previousDirection != direction)
  {
    cumulativeError = 0.0; // reset Integrale
    lastError = targetPosition - position;
  }
  Serial.printf("move receive: %f %f => max power=%f target pos=%f\n",position,power,maxPwm,targetPosition);
}

void LinearActuator::tick()
{
  tickCalls += 1;
  status = LA_STATUS_OK;
  // check nano last time read:
  if ((millis() - lastNanoReadMs) > lostPositionAfterMs)
  {
    status |= LA_STATUS_MISSING_NANO;
    // we possibily lost nano, emergency stop
    currentPwm = 0.0;
    position = -1.0;
    set(0, 0, 0.0);
    return;
  }
  if ((position < 0) || (targetPosition < 0))
  {
    status |= LA_STATUS_STOP;
    currentPwm = 0.0;
    set(0, 0, 0.0);
    return;
  }

  if ((millis() - lastTickMs) < minTickDtMs)
  {
    //if (leg == 1 && actuator == 3)
      //Serial.printf("too soon %d %d %d %d\n",millis(),lastTickMs,millis()-lastTickMs,minTickDtMs);
    return;
  }
  

  float dtsec = (float)(millis() - lastTickMs) / 1000.0;
  nticking += 1;
  lastTickMs = millis();

  positionError =
      targetPosition - position;
  //Serial.printf("%d;%d %lf - %lf => %lf \n",leg,actuator,targetPosition, position , positionError);

  // check if direction changed (either by order or over shoot)
  if ((positionError > 0) && (direction == -1))
  {
    cumulativeError = 0; // reset integral
    direction = 1;       // update direction
  }
  else if ((positionError < 0) && (direction == 1))
  {
    cumulativeError = 0; // reset integral
    direction = -1;      // update direction
  }
  else
  {
    cumulativeError += positionError * dtsec;
    if ((cumulativeError < 0) && (cumulativeError < -maxCumulativeError))
      cumulativeError = -maxCumulativeError;
    if ((cumulativeError > 0) && (cumulativeError > maxCumulativeError))
      cumulativeError = maxCumulativeError;
    //Serial.printf("cum err is %lf dtsec is %lf\n", cumulativeError, dtsec);
  }
  // PID formula:
  float derr = (positionError - lastError) / dtsec;

  targetPwm = Kp * positionError + Ki * cumulativeError + Kd * derr;
  // Serial.printf("%lf = %lf * %lf + %lf * %lf + %lf * %lf\n",targetPwm,Kp,positionError,Ki,cumulativeError,Kd,derr);

  lastError = positionError;

  
  if (fabs(positionError)<targetTolerance){
    targetPwm=0.0;
  }
  /*
  if (fabs(targetPwm)<minPwm){
    targetPwm=0.0;
  }
  */


 
  if (targetPwm > maxPwm)
    targetPwm = maxPwm;
  if (targetPwm < -maxPwm)
    targetPwm = -maxPwm;

  // check on position
  //  - emergency
  if (((position < emergencyStopArea) && (targetPwm < 0)) ||
      ((position > (actuatorLength - emergencyStopArea)) && (targetPwm > 0)))
  {
    status = LA_STATUS_EMERGENCY_AREA;
    currentPwm = 0.0;
    set(0, 0, 0.0);
    return;
  }
  // - safe
  if ((position < safeArea) && (targetPwm < 0))
  {
    status = LA_STATUS_SAFE_AREA;
    targetPwm = std::max(targetPwm, -safePwm);
  }
  if ((position > (actuatorLength - safeArea)) && (targetPwm > 0))
  {
    status = LA_STATUS_SAFE_AREA;
    targetPwm = std::min(targetPwm, safePwm);
  }

  // adjust current pwm according to target and slope
  // print("A p: %f t:%f e:%f tp:%f cp:%f dt:%f\n",position,targetPosition, positionError, targetPwm,currentPwm,dtsec);
  if (currentPwm > targetPwm)
  {
    currentPwm = currentPwm - dtsec / slopePwm;
    if (currentPwm < targetPwm)
      currentPwm = targetPwm;
  }
  else
  {
    currentPwm = currentPwm + dtsec / slopePwm;
    if (currentPwm > targetPwm)
      currentPwm = targetPwm;
  }

  // apply pwm
  if (currentPwm > 0)
    set(0, 1, currentPwm);
  else
    set(1, 0, abs(currentPwm));
  // print("B p: %f t:%f e:%f tp:%f cp:%f dt:%f\n",position,targetPosition, positionError, targetPwm,currentPwm,dtsec);
  // print("%f %f %f %f %f %f\n", position, targetPosition, positionError, targetPwm, currentPwm, dtsec);
}
