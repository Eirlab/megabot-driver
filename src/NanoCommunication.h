#ifndef MEGABOT_DRIVER_MUXCOMMUNICATION_H
#define MEGABOT_DRIVER_MUXCOMMUNICATION_H

#include <chrono>
#include <vector>
#include "crc.h"
#include "Job.h"
#include <Arduino.h> 

void print(const char *msg, ...);

class NanoComCallback {
public:
  virtual ~NanoComCallback() {}
  virtual void value(int leg, int actuator, int value) = 0;
};

class NanoDirectCom : public Job  {
  HardwareSerial *rxChannel;
  NanoComCallback *nextCb;
  char *buffer;
  int bufferSize;
  uint32_t v = 0;

  bool check_nano(uint32_t value, int &legSelected, int &linearActuatorSelected,
                uint16_t &mesureInt);
  public:
  NanoDirectCom(HardwareSerial *rx, NanoComCallback *next);
  virtual const char *name() const;
  virtual void tick() override;
  void resetStats();

  unsigned int parityError=0;
  unsigned int crcError=0;
  unsigned int dataError=0;
  unsigned int valid=0;
};

#endif 
