#include "Parser.hpp"
#include <cstdarg>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define epislon 1e-6

void ParserBase::error(const char *fmt, ...) {
  static char errorbuf[100];
  if (error_cb) {
    va_list l;
    va_start(l, fmt);
    vsnprintf(errorbuf, 100, fmt, l);
    va_end(l);
    error_cb(errorbuf);
  }
}

char ParserTxt::get_order() {
  internal_state = OK;
  static char orders[] = "LPSI";
  static int norders = sizeof(orders) / sizeof(orders[0]);
  if (p >= e) {
    internal_state = WAITING;
    return 0;
  }
  for (int i = 0; i < norders; ++i)
    if (buffer[p] == orders[i]) {
      p += 1;
      return orders[i];
    }
  internal_state = ERROR;
  error("wrong char order: %c (%x)\n", buffer[p], buffer[p]);
  return -1;
}

void ParserTxt::wait_for_carriage_return() {
  internal_state = OK;
  while ((p < e) && (buffer[p] != '\n'))
    p++;
  if (p == e)
    internal_state = WAITING;
  else {
    p += 1;
    current_state = &ParserTxt::main;
  }
}

float ParserTxt::read_float_between_dash(float defaultvalue) {
  float value = -3;
  if (p >= e) {
    internal_state = WAITING;
    return 0;
  }
  if (buffer[p] != '#') {
    internal_state = ERROR;
    error("missing # in read float %c \n", buffer[p]);
    return 0;
  }
  int float_start = p + 1;
  int float_end = p + 1;
  while ((float_end < e) &&
         (isdigit(buffer[float_end]) || (buffer[float_end] == '.')) &&
         (buffer[float_end] != '#'))
    float_end += 1;
  if (float_end == e) {
    internal_state = WAITING;
    return 0;
  }
  if (buffer[float_end] != '#') {
    internal_state = ERROR;
    error("wrong char in float position: %c \n", buffer[float_end]);
    return 0;
  }
  if (float_end == float_start) {
    value = defaultvalue;
  } else {
    value = strtof(buffer + float_start, NULL);
  }
  p = float_end;
  return value;
}

void ParserTxt::parse_L_position() {
  lparam.position = read_float_between_dash(-1);
  if ((internal_state == ERROR) || (internal_state == WAITING))
    return;

  if ((lparam.position != -1) &&
      ((lparam.position < 0) || (lparam.position > (0.2 + epislon)))) {
    internal_state = ERROR;
    error("wrong position order value: %lf\n", lparam.position);
    return;
  }
  current_state = &ParserTxt::parse_L_power;
}
void ParserTxt::parse_L_power() {
  lparam.power = read_float_between_dash(-1);
  if ((internal_state == ERROR) || (internal_state == WAITING))
    return;

  if ((lparam.power != -1) &&
      ((lparam.power < 0) || (lparam.power > (1.0 + epislon)))) {
    internal_state = ERROR;
    error("wrong power order value: %lf\n", lparam.power);
    return;
  }
  if (l_cb)
    l_cb(lparam.leg, lparam.actuator, lparam.position, lparam.power);
  current_state = &ParserTxt::wait_for_carriage_return;
}

void ParserTxt::parse_L_start() {
  lparam = {-1, -1, -1, -1};
  if ((p + 1) >= e) {
    internal_state = WAITING;
    return;
  }
  if ((buffer[p] >= '1') && (buffer[p] <= '4'))
    lparam.leg = buffer[p] - '0';
  else {
    internal_state = ERROR;
    error("wrong leg id: %c -> %d\n", buffer[p], buffer[p] - '0');
    return;
  }
  p += 1;
  if ((buffer[p] >= '1') && (buffer[p] <= '3'))
    lparam.actuator = buffer[p] - '0';
  else {
    internal_state = ERROR;
    error("wrong actuator id: %c -> %d\n", buffer[p], buffer[p] - '0');
    return;
  }
  p += 1;
  current_state = &ParserTxt::parse_L_position;
}
void ParserTxt::parse_P_start() {
  float optional_freq = read_float_between_dash(0.5); // default value
  if ((internal_state == ERROR) || (internal_state == WAITING))
    return;
  if (p_cb)
    p_cb(optional_freq);
  current_state = &ParserTxt::wait_for_carriage_return;
}

void ParserTxt::parse_S_start() {
  if (s_cb)
    s_cb();
  current_state = &ParserTxt::wait_for_carriage_return;
}
void ParserTxt::parse_I_start() {
  if (i_cb)
    i_cb();
  current_state = &ParserTxt::wait_for_carriage_return;
}

void ParserTxt::main() {
  char order = get_order();
  if (internal_state == ERROR)
    if (internal_state == WAITING)
      return;
  if (order == 'L')
    current_state = &ParserTxt::parse_L_start;
  if (order == 'P')
    current_state = &ParserTxt::parse_P_start;
  if (order == 'S')
    current_state = &ParserTxt::parse_S_start;
  if (order == 'I')
    current_state = &ParserTxt::parse_I_start;
}

ParserTxt::ParserTxt(int capacity) : ParserBase(capacity) {
  current_state = &ParserTxt::main;
}

ParserBase::ParserBase(int capacity)
    : buffer(new char[capacity]), blen(capacity), p(0), e(0) {}

ParserBase::~ParserBase() {}

void ParserBase::append(char c) {
  if (e < blen) {
    buffer[e] = c;
    e++;
  }
}
void ParserBase::append(const char *s, int l) {
  for (int i = 0; i < l; ++i) {
    append(s[i]);
  }
  process();
}
void ParserBase::append(const char *s) {
  int i = 0;
  while (s[i] != '\0') {
    append(s[i]);
    i += 1;
  }
  process();
}
void ParserBase::flush_buffer() {
  if ((p < (blen / 2)) && (e < (blen / 2)))
    return;
  int i = 0;
  while (p < e) {
    buffer[i] = buffer[p];
    i += 1;
    p += 1;
  }
  e = i;
  p = 0;
}
void ParserTxt::process() {
  internal_state = OK;
  int t = p;
  while (internal_state != WAITING) {
    ((*this).*current_state)();
    if (internal_state == ERROR) {
      current_state = &ParserTxt::wait_for_carriage_return;
      buffer[e] = 0;
      fprintf(stderr, "buffer: %s\n", buffer + t);
    }
    //      fprintf(stderr,"buffer: %s\n",buffer+p);
  }
  flush_buffer();
}

// Binary input order:

#include "crc.h"

#define ORDER_HEADER 0x52
#define MOVE 1
#define STOP 2
#define PRINT 4

struct Order {
  unsigned char head;
  unsigned char order;
  unsigned char leg;
  unsigned char actuator; // 4 bytes
  unsigned short value;
  unsigned short pwm; // 8 bytes
  unsigned char crc1;
  unsigned char crc2; // 10 bytes
};

ParserBin::ParserBin(int capacity) : ParserBase(capacity) {}
void ParserBin::process() {
  // error("PS1 %d %d %d %d\n",p,e,(e - sizeof(Order)),p <= (e -
  // sizeof(Order)));
  while (p <= (int)(e - sizeof(Order))) {
    // mean that buffer possibly contains an order
    if (buffer[p] == ORDER_HEADER) { // try to extract an order
      error("parser bin: try to parse a data\n");
      struct Order *o = (struct Order *)(buffer + p);
      if ((o->crc1 == get_crc((unsigned char *)o, sizeof(struct Order) - 2)) &&
          (o->crc2 == get_crc((unsigned char *)o, sizeof(struct Order) - 1))) {
        error("parser bin: crc is OK\n");
        // order is valid, CRC are ok
        if (o->order == MOVE) {
          if (this->l_cb)
            this->l_cb(o->leg, o->actuator, ((float)o->value) / 10000.0,
                       ((float)o->pwm) / 10000.0);
        } else if (o->order == STOP) {
          if (this->s_cb)
            this->s_cb();
        } else if (o->order == PRINT) {
          error("receive a print order with value %d \n", o->value);
          if (this->p_cb)
            this->p_cb(((float)o->value) / 10000.0);
        }
        p += sizeof(Order);
      } else
        p += 1;
    }
  }
  // error("PS2\n");

  flush_buffer();
  // error("PS3\n");
}