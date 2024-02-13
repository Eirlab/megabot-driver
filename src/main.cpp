#include "DigitalOut.h"
#include "GlobalConfig.h"
#include "Kernel.h"
#include "LinearActuator.h"
#include "NanoCommunication.h"
#include "Parser.hpp"
#include "PinOut.h"
#include "SerialBase.h"
#include "Timer.h"
#include "mbed.h"
#include "mbed_debug.h"
#include <Filters.hpp>
#include <cctype>
#include <chrono>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ratio>
#include <vector>

/* ************************************************
 *                  Objects
 * ************************************************/
EventFlags Flags;
BufferedSerial serialMaster(USBTX, USBRX, BaudRateMaster);
Kernel::Clock::time_point startTime = Kernel::Clock::now();

void print(const char *msg, ...);

/*
LEG ACTU MIN  MAX
  1    1   0 1023
  1    2  26  973
  1    3   0 1023

  2    1   0 1023
  2    2  35  969
  2    3   0 1023

  3    1   0 1023
  3    2  32 997
  3    3   0 1023

  4    1   0 1023
  4    2  22 954
  4    3   0 1023
*/

unsigned int main_ticking = 0;

#define FREQ 100

ControlerAB C1A(pin_C1A_DIR1, pin_C1A_DIR2, pin_C1A_PWM);
ControlerAB C1B(pin_C1B_DIR1, pin_C1B_DIR2, pin_C1B_PWM);
ControlerAB C2A(pin_C2A_DIR1, pin_C2A_DIR2, pin_C2A_PWM);
ControlerAB C2B(pin_C2B_DIR1, pin_C2B_DIR2, pin_C2B_PWM);
ControlerAB C3A(pin_C3A_DIR1, pin_C3A_DIR2, pin_C3A_PWM);
ControlerAB C3B(pin_C3B_DIR1, pin_C3B_DIR2, pin_C3B_PWM);

class NanosToLinearActuator : public NanoComCallback, public Job {

public:
  LinearActuator *la[4][3];
  unsigned int read_ticking[4][3];
  int setup12 = 0, setup34 = 0;
  bool setupDone = false;

  NanosToLinearActuator() {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j) {
        la[i][j] = nullptr;
        read_ticking[i][j] = 0;
      }
  }
  void addLinearActuator(LinearActuator *actuator) {
    la[actuator->leg - 1][actuator->actuator - 1] = actuator;
  }
  void makeSetup12() {

    addLinearActuator(new LinearActuator(1, 1, C1A, 0, 1023, FREQ));
    addLinearActuator(new LinearActuator(1, 2, C2A, 26, 973, FREQ));
    addLinearActuator(new LinearActuator(1, 3, C3A, 0, 1023, FREQ));

    addLinearActuator(new LinearActuator(2, 1, C1B, 0, 1023, FREQ));
    addLinearActuator(new LinearActuator(2, 2, C3B, 35, 969, FREQ));
    addLinearActuator(new LinearActuator(2, 3, C2B, 0, 1023, FREQ));
    setupDone = true;
  }
  void makeSetup34() { 

addLinearActuator(new LinearActuator(3, 1, C1A, 0, 1023, FREQ));
addLinearActuator(new LinearActuator(3, 2, C2A, 32, 997, FREQ));
addLinearActuator(new LinearActuator(3, 3, C3A, 0, 1023, FREQ));

addLinearActuator(new LinearActuator(4, 1, C1B, 0, 1023, FREQ));
addLinearActuator(new LinearActuator(4, 2, C3B, 24, 1000, FREQ));
addLinearActuator(new LinearActuator(4, 3, C2B, 0, 1023, FREQ));
      setupDone = true; 
      }
  virtual void value(int leg, int actuator, int value) override {
    if (setupDone == false) {
      if ((leg == 1) || (leg == 2))
        setup12 += 1;
      if ((leg == 3) || (leg == 4))
        setup34 += 1;
      if (setup12 > (setup34 + 5)) {
        makeSetup12();
      } else if (setup34 > (setup12 + 5)) {
        makeSetup34();
      }
    } else {
      if (la[leg - 1][actuator - 1] != nullptr) {
        la[leg - 1][actuator - 1]->setPositionNano(value);
      }
    }
  }

  void tick() override {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if (la[i][j] != nullptr)
          la[i][j]->tick();
  }

  void move(int leg, int actuator, float position, float power) {
    if ((leg >= 1) && (leg <= 4) && (actuator >= 1) && (actuator <= 3)) {
      if (la[leg - 1][actuator - 1] != nullptr)
        la[leg - 1][actuator - 1]->moveTo(position, power);
    }
  }
  void print_informations() {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if (la[i][j] != nullptr)
          print("L%d%d#%lf#%lf#%lf#%lf#\n", i + 1, j + 1,
                la[i][j]->getPosition(), la[i][j]->getTargetPosition(),
                la[i][j]->getCurrentPwm(), la[i][j]->getTargetPwm());
        else {
          // print("L%d%d#missing#\n", i + 1, j + 1);
        }
  }

  void binary_print_information() {
#define INFO_HEADER 0xA5
#define INFO_STATUS_MISSING 0xFF

    struct Info {
      unsigned char head;
      unsigned char leg;
      struct {
        unsigned short status;
        short position;
        short target;
        short pwm;
        short pwmtarget;
      } actuators[3];
      unsigned char crc1;
      unsigned char crc2;
    };
    // #define STATS_HEADER 0x68
    //     struct Stats {
    //         unsigned char head;

    //         unsigned char crc;
    //     };
    for (int i = 0; i < 4; ++i) {
      if ((la[i][0] != nullptr) || (la[i][1] != nullptr) ||
          (la[i][2] != nullptr)) {
        struct Info info;
        info.head = INFO_HEADER;
        info.leg = i + 1;
        for (int j = 0; j < 3; ++j) {
          if (la[i][j] != nullptr) {
            info.actuators[j].status = la[i][j]->status;
            info.actuators[j].position = la[i][j]->getPosition() * 10000;
            info.actuators[j].target = la[i][j]->getTargetPosition() * 10000;
            info.actuators[j].pwm = la[i][j]->getCurrentPwm() * 10000;
            info.actuators[j].pwmtarget = la[i][j]->getTargetPwm() * 10000;

          } else {
            info.actuators[j].status = INFO_STATUS_MISSING;
            info.actuators[j].position = 0;
            info.actuators[j].target = 0;
            info.actuators[j].pwm = 0;
            info.actuators[j].pwmtarget = 0;
          }
        }
        info.crc1 = get_crc((unsigned char *)&info, sizeof(info) - 2);
        info.crc2 = get_crc((unsigned char *)&info, sizeof(info) - 1);
        serialMaster.write(&info, sizeof(info));
      }
    }
  }

  void stop() {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if (la[i][j] != nullptr)
          la[i][j]->stop();
  }
};

//#define CONTROLER12

#ifdef CONTROLER12

#else


#endif

NanosToLinearActuator nanosToLinearActuators;

XMedianFilter medianFilter(3, FREQ * 2, &nanosToLinearActuators);

/* ************************************************
 *         MAIN - LinearActuator thread
 * ************************************************/

void print(const char *msg, ...) {
  static char buf[200];
  va_list l;
  va_start(l, msg);
  int r = vsnprintf(buf, 200, msg, l);
  va_end(l);
  serialMaster.write(buf, r);
}

void move_order(int leg, int actuator, float position, float power) {
  print("order %d %d %lf %lf\n", leg, actuator, position, power);
  nanosToLinearActuators.move(leg, actuator, position, power);
}

void parser_error(const char *msg) { print(msg); }

void print_informations() {
  // nanosToLinearActuators.print_informations();
  nanosToLinearActuators.binary_print_information();
}

void binary_print_global_information() {
#define GLOBAL_INFO_HEADER 0x53

  struct GlobalInfo {
    unsigned char head;
    unsigned char padding[3];          // 4
    unsigned int main_ticking;         // 8
    unsigned int actuator_ticking[12]; // 4*12=48 => 56
    unsigned int read_ticking[12];     // +48 => 104
    unsigned char end_padding[2];
    unsigned char crc1;
    unsigned char crc2; // 108
  };
  struct GlobalInfo info;
  info.head = GLOBAL_INFO_HEADER;
  info.padding[0] = 0;
  info.padding[1] = 1;
  info.padding[2] = 2;
  info.main_ticking = main_ticking;
  main_ticking = 0;

  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 3; ++j) {
      if (nanosToLinearActuators.la[i][j] != nullptr) {
        info.actuator_ticking[i * 3 + j] =
            nanosToLinearActuators.la[i][j]->nticking;
        nanosToLinearActuators.la[i][j]->nticking = 0;
      } else {
        info.actuator_ticking[i * 3 + j] = 0;
      }
      info.read_ticking[i * 3 + j] = nanosToLinearActuators.read_ticking[i][j];
    }
  info.end_padding[0] = 0;
  info.end_padding[1] = 1;
  info.crc1 = get_crc((unsigned char *)&info, sizeof(info) - 2);
  info.crc2 = get_crc((unsigned char *)&info, sizeof(info) - 1);
  serialMaster.write(&info, sizeof(info));
}

static double print_freq = 0.0;

auto lastPrint = Kernel::Clock::now();

void set_print_informations_freq(float freq) {
  print_freq = freq;
  lastPrint = Kernel::Clock::now();
}

void stop() { nanosToLinearActuators.stop(); }

#define smwrite(s)                                                             \
  serialMaster.write(s, strlen(s));                                            \
  serialMaster.sync();

int main() {

  smwrite("starting...\n");

  char input[500];
  ParserBin parser(1000);
  parser.l_cb = move_order;
  parser.error_cb = parser_error;
  parser.i_cb = print_informations;
  parser.p_cb = set_print_informations_freq;
  parser.s_cb = stop;

  smwrite("parser setup\n");


  smwrite("creating nano serial... ");

  Job *jobs[5];
  NanoComCallback *nextCb = &nanosToLinearActuators;
  jobs[0] = &nanosToLinearActuators;
  NanoDirectCom dcPd2(PD_2, nextCb);
  jobs[1] = &dcPd2;
  NanoDirectCom dcPa1(PA_1, nextCb);
  jobs[2] = &dcPa1;
  NanoDirectCom dcPc5(PC_5, nextCb);
  jobs[3] = &dcPc5;
  NanoMuxCom mc(nextCb);
  jobs[4] = &mc;
  smwrite("done\n");

  smwrite("main loop...\n");

  auto lastBinaryInfo = Kernel::Clock::now();

  while (true) {
    main_ticking += 1;
    auto n = Kernel::Clock::now();
    if (std::chrono::duration<double>(n - lastBinaryInfo).count() > 1) {
      binary_print_global_information();
      lastBinaryInfo = n;
    }
    // serialMaster.write("B\n", 2);

    for (int j = 0; j < 5; ++j) {
      jobs[j]->tick();
    }

    // serialMaster.write("C\n", 2);

    if (serialMaster.readable()) {
      int c = serialMaster.read(input, 500);
      parser.append(input, c);
    }
    // serialMaster.write("D\n", 2);

    if (print_freq > 0) {
      auto dt = Kernel::Clock::now() - lastPrint;
      if (std::chrono::duration<double>(dt).count() > print_freq) {
        print_informations();
        lastPrint = Kernel::Clock::now();
      }
    }
    // serialMaster.write("E\n", 2);

    //    ThisThread::sleep_for(2ms);
  }
}