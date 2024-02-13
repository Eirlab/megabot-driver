#pragma once

#include "NanoCommunication.h"

class MedianFilter {
  int size;
  int *values;
  int *buffer;
  int position;
  double period;
  rtos::Kernel::Clock::time_point lastCompute;

public:
  MedianFilter(int size, int freq) ;
  bool newValue(int v, int &r) ;
};

class XMedianFilter : public NanoComCallback {
  NanoComCallback *next;
  MedianFilter *filters[4][3];

public:
  virtual ~XMedianFilter() ;
  XMedianFilter(int size, int freq, NanoComCallback *next);

  virtual void value(int leg, int actuator, int value) ;
};