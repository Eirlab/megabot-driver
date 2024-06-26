#pragma once

struct ControlerAB {
  int DIR1;
  int DIR2;
  int PWM;
  ControlerAB(int DIR1, int DIR2, int PWM)
      : DIR1(DIR1), DIR2(DIR2), PWM(PWM) {}
};

