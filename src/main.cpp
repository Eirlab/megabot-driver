#include <Arduino.h>
#include "config.h"
#include "Job.h"
#include <vector>
#include "NanoCommunication.h"
#include "Controler.h"
#include "LinearActuator.h"
#include "Parser.hpp"
#include "Filters.hpp"

#define print(...) Serial.printf(__VA_ARGS__)

HardwareSerialIMXRT *s[] = {&Serial1, &Serial3, &Serial4, &Serial5, &Serial7, &Serial8};
NanoDirectCom *nanoCom[sizeof(s) / sizeof(s[0])];

#define FREQ 100

ControlerAB C1A(pin_C1A_DIR1, pin_C1A_DIR2, pin_C1A_PWM);
ControlerAB C1B(pin_C1B_DIR1, pin_C1B_DIR2, pin_C1B_PWM);
ControlerAB C2A(pin_C2A_DIR1, pin_C2A_DIR2, pin_C2A_PWM);
ControlerAB C2B(pin_C2B_DIR1, pin_C2B_DIR2, pin_C2B_PWM);
ControlerAB C3A(pin_C3A_DIR1, pin_C3A_DIR2, pin_C3A_PWM);
ControlerAB C3B(pin_C3B_DIR1, pin_C3B_DIR2, pin_C3B_PWM);

ControlerAB *allControlers[] = {&C1A, &C1B, &C2A, &C2B, &C3A, &C3B};

class NanosToLinearActuator : public NanoComCallback, public Job
{

public:
  LinearActuator *la[4][3];
  unsigned int read_ticking[4][3];
  int setup12 = 0, setup34 = 0;
  bool setupDone = false;

  NanosToLinearActuator()
  {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
      {
        la[i][j] = nullptr;
        read_ticking[i][j] = 0;
      }
    // DEBUG!!!
    // makeSetup12();
    //  TO REMOVE
  }
  void addLinearActuator(LinearActuator *actuator)
  {
    la[actuator->leg - 1][actuator->actuator - 1] = actuator;
  }

  virtual const char *name() const
  {
    return "nanoToLA";
  }

  void makeSetup12()
  {

    addLinearActuator(new LinearActuator(1, 1, C1A, 0, 1023, FREQ));
    addLinearActuator(new LinearActuator(1, 2, C2A, 7, 964, FREQ));
    addLinearActuator(new LinearActuator(1, 3, C3A, 0, 1023, FREQ));

    addLinearActuator(new LinearActuator(2, 1, C1B, 0, 1023, FREQ));
    addLinearActuator(new LinearActuator(2, 2, C3B, 35, 969, FREQ));
    addLinearActuator(new LinearActuator(2, 3, C2B, 0, 1023, FREQ));
    setupDone = true;
  }
  void makeSetup34()
  {

    addLinearActuator(new LinearActuator(3, 1, C1A, 0, 1023, FREQ));
    addLinearActuator(new LinearActuator(3, 2, C2A, 32, 997, FREQ));
    addLinearActuator(new LinearActuator(3, 3, C3A, 0, 1023, FREQ));

    addLinearActuator(new LinearActuator(4, 1, C1B, 0, 1023, FREQ));
    addLinearActuator(new LinearActuator(4, 2, C3B, 24, 1000, FREQ));
    addLinearActuator(new LinearActuator(4, 3, C2B, 0, 1023, FREQ));
    setupDone = true;
  }
  virtual void value(int leg, int actuator, int value) override
  {
    if (setupDone == false)
    {
      if ((leg == 1) || (leg == 2))
        setup12 += 1;
      if ((leg == 3) || (leg == 4))
        setup34 += 1;
      if (setup12 > (setup34 + 5))
      {
        makeSetup12();
      }
      else if (setup34 > (setup12 + 5))
      {
        makeSetup34();
      }
    }
    else
    {
      if (la[leg - 1][actuator - 1] != nullptr)
      {
        la[leg - 1][actuator - 1]->setPositionNano(value);
      }
    }
  }

  void tick() override
  {
    if (setupDone == false)
      return;
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if (la[i][j] != nullptr)
          la[i][j]->tick();
  }
  void move(int leg, int actuator, float position, float power)
  {
    if ((leg >= 1) && (leg <= 4) && (actuator >= 1) && (actuator <= 3))
    {
      if (la[leg - 1][actuator - 1] != nullptr)
        la[leg - 1][actuator - 1]->moveTo(position, power);
    }
  }
  void print_informations()
  {
    if (setupDone == false)
    {
      print("setup in progress: 12(%d) 34(%d)\n", setup12, setup34);
      return;
    }
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if ((la[i][j] != nullptr) && ((la[i][j]->status & LA_STATUS_OK) == LA_STATUS_OK))
        {
          print("L%d%d#%lf(%d)#%lf#%lf#%lf#", i + 1, j + 1,
                la[i][j]->getPosition(), la[i][j]->posNanoRaw, la[i][j]->getTargetPosition(),
                la[i][j]->getCurrentPwm(), la[i][j]->getTargetPwm());
          print("#tick(%d;%d;%d)#posErr(%.3lf)#lastErr(%.3lf)#cumErr(%.3lf)#dir(%d)",
                la[i][j]->nticking, la[i][j]->lastTickMs, la[i][j]->tickCalls, la[i][j]->positionError, la[i][j]->lastError,
                la[i][j]->cumulativeError, la[i][j]->direction);
          if (la[i][j]->status & LA_STATUS_OK)
            print(" ok ");
          if (la[i][j]->status & LA_STATUS_MISSING)
            print(" missing ");
          if (la[i][j]->status & LA_STATUS_MISSING_NANO)
            print(" nano_missing ");
          if (la[i][j]->status & LA_STATUS_SAFE_AREA)
            print(" safe_area ");
          if (la[i][j]->status & LA_STATUS_EMERGENCY_AREA)
            print(" emergency_area ");
          if (la[i][j]->status & LA_STATUS_STOP)
            print(" stop ");
          print("\n");
        }
        else
        {
          // print("L%d%d#missing#\n", i + 1, j + 1);
        }
  }
  void binary_print_information()
  {
    /*
    if (setupDone == false)
    {
      print("setup in progress: 12(%d) 34(%d)\n", setup12, setup34);
      return;
    }
    */
#define INFO_HEADER 0xA5
#define INFO_STATUS_MISSING 0xFF

    struct Info
    {
      unsigned char head;
      unsigned char leg;
      struct
      {
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
    for (int i = 0; i < 4; ++i)
    {
      if ((la[i][0] != nullptr) || (la[i][1] != nullptr) ||
          (la[i][2] != nullptr)) // Send info only about contoler legs (12 or 34)
      {
        struct Info info;
        info.head = INFO_HEADER;
        info.leg = i + 1;
        for (int j = 0; j < 3; ++j)
        {
          if (la[i][j] != nullptr)
          {
            // print("bpi %d %d %f %f
            // \n",i,j,la[i][j]->position,la[i][j]->targetPosition);
            info.actuators[j].status = la[i][j]->status;
            info.actuators[j].position = la[i][j]->getPosition() * 10000;
            info.actuators[j].target = la[i][j]->getTargetPosition() * 10000;
            info.actuators[j].pwm = la[i][j]->getCurrentPwm() * 10000;
            info.actuators[j].pwmtarget = la[i][j]->getTargetPwm() * 10000;
          }
          else
          {
            info.actuators[j].status = INFO_STATUS_MISSING;
            info.actuators[j].position = 0;
            info.actuators[j].target = 0;
            info.actuators[j].pwm = 0;
            info.actuators[j].pwmtarget = 0;
          }
        }
        info.crc1 = get_crc((unsigned char *)&info, sizeof(info) - 2);
        info.crc2 = get_crc((unsigned char *)&info, sizeof(info) - 1);
        Serial.write((const uint8_t *)&info, sizeof(info));
      }
    }
  }

  void stop()
  {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 3; ++j)
        if (la[i][j] != nullptr)
          la[i][j]->stop();
  }
};

class NanoDebug : public NanoComCallback
{
public:
  virtual void value(int leg, int actuator, int value) override
  {
    Serial.printf("nano: %d %d %d\n", leg, actuator, value);
  }
};

std::vector<Job *> jobs;

NanoDebug nanoDebug;
NanosToLinearActuator nanosToLinearActuators;
XMedianFilter medianFilter(5, 4 * FREQ, &nanosToLinearActuators);

uint32_t loopTime;
char input[500];
ParserBin parser(1000);
static uint32_t print_freq = 0; // in ms
uint32_t lastPrint;

void set_print_informations_freq(uint32_t freq)
{
  print_freq = freq;
  lastPrint = millis();
}

/* parser callbacks: */
void parser_error(const char *msg) { print(msg); }
void print_informations()
{
  // nanosToLinearActuators.print_informations();
  nanosToLinearActuators.binary_print_information();
}
void stop() { nanosToLinearActuators.stop(); }
void move_order(int leg, int actuator, float position, float power)
{
  // print("order: %d %d %f %f\n", leg, actuator, position, power);
  nanosToLinearActuators.move(leg, actuator, position, power);
}

void setup()
{
  analogWriteResolution(HW_PWM_RESOLUTION);
  Serial.begin(MASTER_BAUDRATE);
  Serial.setTimeout(0);

  for (auto p : allControlers)
  {
    pinMode(p->DIR1, OUTPUT);
    pinMode(p->DIR2, OUTPUT);
    analogWriteFrequency(p->PWM, HW_PWM_FREQ);
  }

  for (unsigned int i = 0; i < sizeof(s) / sizeof(s[0]); ++i)
  {
    s[i]->begin(NANO_BAUDRATE);
    // jobs.push_back(new NanoDirectCom(s[i], &nanoDebug));
    nanoCom[i] = new NanoDirectCom(s[i], &medianFilter);
    jobs.push_back(nanoCom[i]);
  }
  jobs.push_back(&nanosToLinearActuators);

  // set parser callbacks:
  parser.l_cb = move_order;
  // parser.error_cb = parser_error;
  parser.i_cb = print_informations;
  parser.p_cb = set_print_informations_freq;
  parser.s_cb = stop;
  // parser.pid_cb = pid_order;

  loopTime = millis();
  Serial.printf("### SETUP FINISHED ###\n");
}

/* Main Loop */
unsigned long loopCounter = 0;

// STATS:
// #define NANO_STATS
// #define JOB_STATS
// #define ACTUATOR_STATS

#ifdef NANO_STATS
auto nanoTime = millis();
#endif

void loop()
{
/** MAIN LOOP ***/
#ifdef NANO_STATS
  if ((millis() - nanoTime) > 1000)
  {
    Serial.printf("\nNano com stats:\n");
    for (unsigned int i = 0; i < sizeof(nanoCom) / sizeof(nanoCom[0]); ++i)
    {
      Serial.printf("%d : parity %d crc %d data %d ok %d\n", i, nanoCom[i]->parityError, nanoCom[i]->crcError, nanoCom[i]->dataError, nanoCom[i]->valid);
      nanoCom[i]->resetStats();
    }
    nanoTime = millis();
  }
#endif

  loopCounter += 1;
  // Serial.printf("hello\n");

  for (auto &j : jobs)
  {
    auto t = millis();
    j->tick();
#ifdef JOB_STATS
    if ((millis() - t) > 1)
    {
      Serial.printf("***** job %s %p take %d ms *****\n", j->name(), millis() - t);
    }
#endif
  }

#ifdef ACTUATOR_STATS
  if ((millis() - loopTime) > 1000)
  {
    Serial.printf("loop counter= %d \n", loopCounter);
    nanosToLinearActuators.print_informations();
    loopTime = millis();
    loopCounter = 0;
  }
#endif

  if ((Serial.available()) > 0)
  {
    int r = Serial.readBytes(input, sizeof(input));
    parser.append(input, r);
  }

  if (print_freq > 0)
  {
    if ((millis() - lastPrint) > print_freq)
    {
      // Serial.printf("send informations...\n");
      print_informations();
      lastPrint = millis();
    }
  }

  /**** CODE BELOW IS FOR CHECKING DRIVERS PINS ****/
  /***
  //ControlerAB &c = C3B; // => ok 4-2
  //ControlerAB &c = C3A;// => ok 3-3
  //ControlerAB &c = C1A;// => ok 3-1
  //ControlerAB &c = C1B;// => ok 4-1
  //ControlerAB &c = C2A;// => ok 3-2
  //ControlerAB &c = C2B;// => ok 4-3

  Serial.printf("stopping controler...\n");
  digitalWrite(c.DIR1, LOW);
  digitalWrite(c.DIR2, LOW);
   analogWrite(c.PWM, 0);
  delay(2000);

  Serial.printf("moving forward at 50%...\n");
  digitalWrite(c.DIR1, LOW);
  digitalWrite(c.DIR2, HIGH);
  analogWrite(c.PWM, HW_PWM_MAX_VALUE/2);
  delay(500);

  Serial.printf("stopping controler...\n");
  digitalWrite(c.DIR1, LOW);
  digitalWrite(c.DIR2, LOW);
   analogWrite(c.PWM, 0);
  delay(2000);

  Serial.printf("moving backward at 50%...\n");
  digitalWrite(c.DIR1, HIGH);
  digitalWrite(c.DIR2, LOW);
  analogWrite(c.PWM, HW_PWM_MAX_VALUE/2);
  delay(500);

  **/
}
