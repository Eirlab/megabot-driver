#include "NanoCommunication.h"
#include "BufferedSerial.h"
#include "GlobalConfig.h"
#include "Kernel.h"
#include "PinNames.h"
#include "PinOut.h"
#include "Thread.h"
#include <cstdint>

/* Following structure map nano 32bits data */
struct nanoValue {
  uint32_t value : 10;
  uint32_t actuatorId : 2;
  uint32_t legId : 3;
  uint32_t pbit : 1;
  uint32_t check : 8;
  uint32_t header : 8;
};

/* Check that value is ok (parity bit and CRC ok), if so: update leg,actu and
   mesure and return true else return false
*/
bool check_nano(uint32_t value, int &legSelected, int &linearActuatorSelected,
                uint16_t &mesureInt) {
  linearActuatorSelected = -1;
  legSelected = -1;
  struct nanoValue *v = (struct nanoValue *)&value;
  if (v->header == 0x9b) {
    uint32_t pbit = v->value ^ (v->value >> 1);
    pbit = pbit ^ (pbit >> 2);
    pbit = pbit ^ (pbit >> 4);
    pbit = pbit ^ (pbit >> 8);
    pbit = pbit & 0x1;
    if ((v->pbit == pbit) &&
        (v->check == get_crc(value & 0xFF, (value >> 8) & 0xFF))) {
      if ((v->legId >= 1) && (v->legId <= 4))
        legSelected = v->legId;
      else
        return false;
      if ((v->actuatorId >= 1) && (v->actuatorId <= 3))
        linearActuatorSelected = v->actuatorId;
      else
        return false;
      mesureInt = v->value;
      return true;
    } else {
    }
  }
  return false;
}

NanoDirectCom::NanoDirectCom(PinName rxPin, NanoComCallback *next) {
  this->nextCb = next;
  rxChannel = new BufferedSerial(NC, rxPin, BaudRateNano);
  bufferSize = 500;
  buffer = new char[bufferSize];
  startTime = Kernel::Clock::now();
}

void NanoDirectCom::tick() {
  int linearActuatorSelected;
  int legSelected;
  uint16_t mesureInt;
  uint32_t v = 0;
  unsigned char c = 0;
  auto t = Kernel::Clock::now();
  int validRead = 0;
  if (rxChannel->readable()) {
    int r = rxChannel->read(buffer, bufferSize);
    for (int l = 0; l < r;) {
      c = buffer[l];
      v = (v >> 8) | (c << 24);
      if (c == 0x9b) {
        if (check_nano(v, legSelected, linearActuatorSelected, mesureInt)) {
          auto t = Kernel::Clock::now();
        //  print("%d;%d;%lf;%d\n", legSelected, linearActuatorSelected,
        //        std::chrono::duration<double>(t - startTime).count(),
        //        mesureInt);
          if (nextCb != nullptr)
            nextCb->value(legSelected, linearActuatorSelected, mesureInt);
          // rxChannel->sync();
          l += 4;
          v = 0;
          validRead += 1;
          if (validRead > 5)
            return;
        } else {
          l += 1;
          // print("read on chan B check wrong ! %x => %x\n", cB, vB);
        }
      } else {
        l += 1;
      }
    }
  }
}

NanoMuxCom::NanoMuxCom(NanoComCallback *cb) {
  muxA = new DigitalOut(pin_Mux_A);
  muxB = new DigitalOut(pin_Mux_B);
  muxC = new DigitalOut(pin_Mux_C);
  rxChannelMux = new BufferedSerial(NC, pin_Mux_Nano_RX, BaudRateNano);
  this->cb = cb;
  this->bufferSize = 500;
  this->buffer = new char[this->bufferSize];
  this->wordAB = 0;
  lastChange = Kernel::Clock::now();
}

void NanoMuxCom::tick() {
  int linearActuatorSelected;
  int legSelected;
  uint16_t mesureInt;
  uint32_t vA = 0;
  unsigned char cA = 0;
  int valueRead = 0;
  int nNanos = 3;
  if (rxChannelMux->readable()) {
    int r = rxChannelMux->read(buffer, bufferSize);
    //print("mux read %r data\n",r);
    for (int l = 0; (l < r) && (valueRead < 5);) {
      cA = buffer[l];
      vA = (vA >> 8) | (cA << 24);
      if (cA == 0x9b &&
          check_nano(vA, legSelected, linearActuatorSelected, mesureInt)) {
        valueRead += 1;
        //print("mux: %d;%d;%d\n", legSelected, linearActuatorSelected, mesureInt);

        if (cb != nullptr)
          cb->value(legSelected, linearActuatorSelected, mesureInt);
        l += 4;
        vA = 0;
      } else {
        l += 1;
      }
    }
  }

  if ((valueRead > 0) || ((Kernel::Clock::now() - lastChange) > 1ms)) {
    wordAB = (wordAB + 1) % nNanos;
    muxA->write(wordAB & 0x1);
    muxB->write((wordAB >> 1) & 0x1);
    muxC->write((wordAB >> 2) & 0x1);
    //print("switch chan %d \n",wordAB);
    rxChannelMux->sync();
    valueRead = false;
    vA = 0;
    lastChange = Kernel::Clock::now();
  }
}
