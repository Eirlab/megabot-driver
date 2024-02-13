#pragma once

class Job {
public:
  virtual void tick() = 0;
  virtual ~Job();
};
