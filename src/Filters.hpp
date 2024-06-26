#pragma once

#include "NanoCommunication.h"

class MedianFilter {
  int size;
  int *values;
  int *buffer;
  int position;
  uint32_t periodMs;
  uint32_t lastComputeMs;

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