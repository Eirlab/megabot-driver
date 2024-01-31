#include "DigitalOut.h"
#include "GlobalConfig.h"
#include "LinearActuator.h"
#include "MuxCommunication.h"
#include "Parser.hpp"
#include "PinOut.h"
#include "SerialBase.h"
#include "Timer.h"
#include "mbed.h"
#include "mbed_debug.h"
#include <cctype>
#include <chrono>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ratio>

/* ************************************************
 *                  Objects
 * ************************************************/
EventFlags Flags;
BufferedSerial serialMaster(USBTX, USBRX, BaudRateMaster);

void print(const char *msg, ...);
/*
LinearActuator baseLegA(baseLA, pin_LegA_baseLeg_PWM, pin_LegA_baseLeg_DIR1,
                        pin_LegA_baseLeg_DIR2, LegA_baseLeg_Pos_Min,
                        LegA_baseLeg_Pos_Max, &Flags);

LinearActuator middleLegA(middleLA, pin_LegA_middleLeg_PWM,
                          pin_LegA_middleLeg_DIR1, pin_LegA_middleLeg_DIR2,
                          LegA_middleLeg_Pos_Min, LegA_middleLeg_Pos_Max,
                          &Flags);

LinearActuator endLegA(endLA, pin_LegA_endLeg_PWM, pin_LegA_endLeg_DIR1,
                       pin_LegA_endLeg_DIR2, LegA_endLeg_Pos_Min,
                       LegA_endLeg_Pos_Max, &Flags);

LinearActuator baseLegB(baseLA, pin_LegB_baseLeg_PWM, pin_LegB_baseLeg_DIR1,
                        pin_LegB_baseLeg_DIR2, LegB_baseLeg_Pos_Min,
                        LegB_baseLeg_Pos_Max, &Flags);

LinearActuator middleLegB(middleLA, pin_LegB_middleLeg_PWM,
                          pin_LegB_middleLeg_DIR1, pin_LegB_middleLeg_DIR2,
                          LegB_middleLeg_Pos_Min, LegB_middleLeg_Pos_Max,
                          &Flags);

LinearActuator endLegB(endLA, pin_LegB_endLeg_PWM, pin_LegB_endLeg_DIR1,
                       pin_LegB_endLeg_DIR2, LegB_endLeg_Pos_Min,
                       LegB_endLeg_Pos_Max, &Flags);
*/
// TODO: Ici on met à jour l'id de la Leg et on va vérifier dans GlobalConfig
// que les flags soient bien configurés
/*
Leg legA(leg1, &baseLegA, &middleLegA, &endLegA);
Leg legB(leg2, &baseLegB, &middleLegB, &endLegB);

Leg *legs[4] = {nullptr, nullptr, nullptr, nullptr};

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
  4    2  24 1000
  4    3   0 1023
*/

class DebugNanos : public MuxComCallback {
  char buffer[500];
  int nread[4 * 3];
  Kernel::Clock::time_point dt[4 * 3];
  Timer last[4 * 3];
  double mindt[4 * 3];
  double maxdt[4 * 3];

public:
  DebugNanos() {
    for (int i = 0; i < 4 * 3; ++i) {
      nread[i] = 0;
      dt[i] = Kernel::Clock::now();
      last[i].start();
      mindt[i] = 10.0;
      maxdt[i] = 0.0;
    }
  }
  virtual void value(int leg, int actuator, int value) override {
    int id = (leg - 1) * 3 + actuator - 1;
    nread[id] += 1;
    auto n = Kernel::Clock::now();
    last[id].stop();
    double d = std::chrono::duration<double>(last[id].elapsed_time()).count();
    mindt[id] = min(mindt[id], d);
    maxdt[id] = max(maxdt[id], d);

    auto x = Kernel::Clock::now() - dt[id];
    if (std::chrono::duration<double>(x).count() > 1) {
      //   if (nread[id] == 50) {
      //    auto x = Kernel::Clock::now() - dt[id];
      int l = snprintf(
          buffer, 500,
          "*** leg %d actuator %d  value %d %lf s (%lf / %lf) %d\n", leg,
          actuator, value, std::chrono::duration<double>(x).count() / nread[id],
          mindt[id], maxdt[id], nread[id]);
      dt[id] = Kernel::Clock::now();
      nread[id] = 0;
      mindt[id] = 10.0;
      maxdt[id] = 0.0;
      serialMaster.write(buffer, l);
    }

    last[id].reset();
    last[id].start();
  }
};
DebugNanos debugNanos;

class NanosToLinearActuator : public MuxComCallback {
  LinearActuator *la[4][3];

public:
  NanosToLinearActuator() {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        la[i][j] = nullptr;
  }
  void addLinearActuator(LinearActuator *actuator) {
    la[actuator->leg - 1][actuator->actuator - 1] = actuator;
  }
  virtual void value(int leg, int actuator, int value) override {
    if (la[leg - 1][actuator - 1] != nullptr) {
      la[leg - 1][actuator - 1]->setPositionNano(value);
    }
  }
  void tick() {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if (la[i][j] != nullptr)
          la[i][j]->tick();
  }
  void move(int leg, int actuator, float position, float power) {
    if ((leg >= 1) && (leg <= 4) && (actuator >= 1) && (actuator <=3)) {
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
        struct Info info = {.head = INFO_HEADER};
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

#define FREQ 100

ControlerAB C1A(pin_C1A_DIR1, pin_C1A_DIR2, pin_C1A_PWM);
ControlerAB C1B(pin_C1B_DIR1, pin_C1B_DIR2, pin_C1B_PWM);
ControlerAB C2A(pin_C2A_DIR1, pin_C2A_DIR2, pin_C2A_PWM);
ControlerAB C2B(pin_C2B_DIR1, pin_C2B_DIR2, pin_C2B_PWM);
ControlerAB C3A(pin_C3A_DIR1, pin_C3A_DIR2, pin_C3A_PWM);
ControlerAB C3B(pin_C3B_DIR1, pin_C3B_DIR2, pin_C3B_PWM);

//#define CONTROLER12

#ifdef CONTROLER12

LinearActuator la11(1, 1, C1A, 0, 1023, FREQ);
LinearActuator la12(1, 2, C2A, 26, 973, FREQ);
LinearActuator la13(1, 3, C3A, 0, 1023, FREQ);

LinearActuator la21(2, 1, C1B, 0, 1023, FREQ);
LinearActuator la22(2, 2, C3B, 35, 969, FREQ);
LinearActuator la23(2, 3, C2B, 0, 1023, FREQ);

#else

LinearActuator la11(3, 1, C1A, 0, 1023, FREQ);
LinearActuator la12(3, 2, C2A, 32, 997, FREQ);
// LinearActuator la13(3, 3, C3A, 0, 1023, FREQ);
LinearActuator la13(3, 3, C2B, 0, 1023, FREQ);

LinearActuator la21(4, 1, C1B, 0, 1023, FREQ);
LinearActuator la22(4, 2, C3B, 24, 1000, FREQ);
// LinearActuator la23(4, 3, C2B, 0, 1023, FREQ);
LinearActuator la23(4, 3, C3A, 0, 1023, FREQ);

#endif

NanosToLinearActuator nanosToLinearActuators;

int cmpint(const int *a, const int *b) {
  if (*a < *b)
    return -1;
  if (*a == *b)
    return 0;
  return 1;
}

class KMeanFilter : public MuxComCallback {
  MuxComCallback *next;
  int size;
  int *values[4][3];
  int pos[4][3];
  int *buf;

public:
  virtual ~KMeanFilter() {

    for (int l = 0; l < 4; ++l)
      for (int a = 0; a < 3; ++a)
        delete[] values[l][a];
  }
  KMeanFilter(int size, MuxComCallback *next) : next(next), size(size) {
    for (int l = 0; l < 4; ++l)
      for (int a = 0; a < 3; ++a) {
        pos[l][a] = 0;
        values[l][a] = new int[size];
        for (int s = 0; s < size; ++s)
          values[l][a][s] = 0;
      }
    buf = new int[size];
  }

  virtual void value(int leg, int actuator, int value) {
    int l = leg - 1;
    int a = actuator - 1;
    values[l][a][pos[l][a]] = value;
    pos[l][a] = (pos[l][a] + 1) % size;
    for (int s = 0; s < size; ++s)
      buf[s] = values[l][a][s];
    std::qsort(buf, size, sizeof(buf[0]),
               (int (*)(const void *, const void *))cmpint);
    next->value(leg, actuator, buf[size / 2]);
  }
};

KMeanFilter kmeanFilter(7, &nanosToLinearActuators);

MuxCommunication muxCom(&Flags, &kmeanFilter);

/* ************************************************
 *         MAIN - LinearActuator thread
 * ************************************************/
// void tickerWorker(){

// }
/*
char dbg_buffer[500];
void debug(const char *msg, ...) {
  va_list l;
  va_start(l, msg);
  int r = vsnprintf(dbg_buffer, 500, msg, l);
  va_end(l);
  serialMaster.write(dbg_buffer, r);
}*/

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

static double print_freq = 0.0;

auto lastPrint = Kernel::Clock::now();

void set_print_informations_freq(float freq) {
  print_freq = freq;
  lastPrint = Kernel::Clock::now();
}

void stop() { nanosToLinearActuators.stop(); }

int main() {
  char input[500];
  ParserBin parser(1000);
  parser.l_cb = move_order;
  parser.error_cb = parser_error;
  parser.i_cb = print_informations;
  parser.p_cb = set_print_informations_freq;
  parser.s_cb = stop;

  //  serialMaster.set_blocking(false);
  nanosToLinearActuators.addLinearActuator(&la11);
  nanosToLinearActuators.addLinearActuator(&la12);
  nanosToLinearActuators.addLinearActuator(&la13);
  nanosToLinearActuators.addLinearActuator(&la21);
  nanosToLinearActuators.addLinearActuator(&la22);
  nanosToLinearActuators.addLinearActuator(&la23);

  muxCom.run();

  serialMaster.write("HELLO\n", 6);

  auto lastDt = Kernel::Clock::now();

  while (true) {
    //serialMaster.write("A\n", 2);
    auto n = Kernel::Clock::now();
    if (std::chrono::duration<double>(n - lastDt).count() > 1) {
      print("com: %d success / %d failed  => %lf %%\n", muxCom.success,
            muxCom.failed,
            100.0 * (double)muxCom.failed / (double)muxCom.success);
      lastDt = n;
    }
    //serialMaster.write("B\n", 2);

    nanosToLinearActuators.tick();
    //serialMaster.write("C\n", 2);

    if (serialMaster.readable()) {
      int c = serialMaster.read(input, 500);
      // input[c] = '\0';
      // print("input:%s\n", input);
      parser.append(input, c);
    }
    //serialMaster.write("D\n", 2);

    if (print_freq > 0) {
      auto dt = Kernel::Clock::now() - lastPrint;
      if (std::chrono::duration<double>(dt).count() > print_freq) {
        print_informations();
        lastPrint = Kernel::Clock::now();
      }
    }
    //serialMaster.write("E\n", 2);

    //    ThisThread::sleep_for(2ms);
  }
  serialMaster.write("F\n", 2);
}