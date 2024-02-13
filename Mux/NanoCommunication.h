#ifndef MEGABOT_DRIVER_MUXCOMMUNICATION_H
#define MEGABOT_DRIVER_MUXCOMMUNICATION_H

#include "GlobalConfig.h"
#include "PinNames.h"
#include "mbed.h"
#include <chrono>
#include <vector>
#include "crc.h"
#include "Job.h"


extern Kernel::Clock::time_point startTime;
extern BufferedSerial serialMaster;

void print(const char *msg, ...);
class NanoComCallback {
public:
  virtual ~NanoComCallback() {}
  virtual void value(int leg, int actuator, int value) = 0;
};

class NanoDirectCom : public Job{
  BufferedSerial *rxChannel;
  NanoComCallback *nextCb;
  char *buffer;
  int bufferSize;
  public:
  NanoDirectCom(PinName rxPin, NanoComCallback *next);
  virtual void tick() override;
};


class NanoMuxCom : public Job{
public:
  NanoMuxCom(NanoComCallback *cb);
  virtual void tick() override;

private:
  DigitalOut *muxA;
  DigitalOut *muxB;
  DigitalOut *muxC;
  BufferedSerial *rxChannelMux;
  char *buffer;
  int bufferSize;
  uint8_t wordAB ;
  Kernel::Clock::time_point lastChange;

  NanoComCallback *cb;
};

#endif 
