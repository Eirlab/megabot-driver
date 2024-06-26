#include "Filters.hpp"

int cmpint(const int *a, const int *b) {
  if (*a < *b)
    return -1;
  if (*a == *b)
    return 0;
  return 1;
}

MedianFilter::MedianFilter(int size, int freq) {
  this->size = size;
  values = new int[size];
  buffer = new int[size];
  position = 0;
  for (int i = 0; i < size; ++i) {
    values[i] = 0;
    buffer[i] = 0;
  }
  periodMs = 1000.0 / ((double)freq);
  lastComputeMs = millis();
}

bool MedianFilter::newValue(int v, int &r) {
  values[position] = v;
  position = (position + 1) % size;
  if ((millis()- lastComputeMs) > periodMs) {
    lastComputeMs = millis();
    for (int i = 0; i < size; ++i)
      buffer[i] = values[i];
    std::qsort(buffer, size, sizeof(buffer[0]),
               (int (*)(const void *, const void *))cmpint);
    r = buffer[size / 2];

    return true;
  }
  return false;
}

XMedianFilter::~XMedianFilter() {
  for (int l = 0; l < 4; ++l)
    for (int a = 0; a < 3; ++a) {
      delete filters[l][a];
    }
}
XMedianFilter::XMedianFilter(int size, int freq, NanoComCallback *next)
    : next(next) {
  for (int l = 0; l < 4; ++l)
    for (int a = 0; a < 3; ++a) {
      filters[l][a] = new MedianFilter(size, freq);
    }
}

void XMedianFilter::value(int leg, int actuator, int value) {
  int l = leg - 1;
  int a = actuator - 1;
  int newvalue;
  //Serial.printf("new value for [%d,%d]: %d\n",leg,actuator,value);
  if (filters[l][a]->newValue(value, newvalue)) {
    //Serial.printf("computing value [%d,%d]: %d => %d\n",leg,actuator,value,newvalue);
    next->value(leg, actuator, newvalue);
  }
}