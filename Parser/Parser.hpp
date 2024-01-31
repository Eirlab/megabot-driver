#pragma once

/** Ad-hoc parser for master serial input strings

read orders:
  move a actuator:
    L[1-4][1-3]#({float})?#({float})?#
    L leg_id actuator_id # position # power #
    position and power can be optional
    valid example:
      L12#40#0.5#
  controle print freq (actuator status):
    P#({float}?)#
    P # frequency #
    valid example:
      P##  => enable at default freq
      P#0.2#  => print every 0.2s
      P#0# => disable print
  emergency stop:
    S
  single information:
    I
orders are separated by \n, in case of parsing error, parser will flush
chars until next \n then start parsing again
*/

class ParserBase {
protected:
  char *buffer;
  int blen;
  int p, e;
  virtual void process() = 0;

  void flush_buffer();

  void error(const char *fmt, ...);
public:
  ParserBase(int capacity = 500);
  virtual ~ParserBase();
  void (*error_cb)(const char *) = nullptr;
  void (*l_cb)(int, int, float, float) = nullptr;
  void (*p_cb)(float) = nullptr;
  void (*s_cb)() = nullptr;
  void (*i_cb)() = nullptr;

  void append(char c);
  void append(const char *s, int l);
  void append(const char *s);
};

class ParserBin : public ParserBase{
public:
ParserBin(int capacity);
virtual void process();
};

class ParserTxt : public ParserBase{
  char *buffer;
  int blen;
  int p, e;
  void (ParserTxt::*current_state)(void);

public:
  void (*error_cb)(const char *) = nullptr;
  void (*l_cb)(int, int, float, float) = nullptr;
  void (*p_cb)(float) = nullptr;
  void (*s_cb)() = nullptr;
  void (*i_cb)() = nullptr;

private:
  enum state {
    OK,
    WAITING,
    ERROR,
  };

  state internal_state = OK;

  struct Lparam {
    int leg, actuator;
    float position, power;
  };
  Lparam lparam;


  char get_order();

  void wait_for_carriage_return();

  float read_float_between_dash(float defaultvalue);
  void parse_L_position();
  void parse_L_power();

  void parse_L_start();
  void parse_P_start();
  void parse_S_start();
  void parse_I_start();
  void main();

public:
  ParserTxt(int capacity = 500);
  virtual void process();
};
