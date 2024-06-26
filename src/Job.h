#pragma once

class Job
{
public:
  virtual const char *name() const = 0;
  virtual void tick() = 0;
  virtual ~Job();
};
