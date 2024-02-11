#include "MuxCommunication.h"
#include "BufferedSerial.h"
#include "Kernel.h"
#include "PinNames.h"
#include "PinOut.h"
#include "Thread.h"
#include <cstdint>

DigitalOut led(LED1);

void print(const char *msg, ...);

MuxCommunication::MuxCommunication(EventFlags *mainFlags, MuxComCallback *cb) {
  MuxA = new DigitalOut(pin_Mux_A);
  MuxB = new DigitalOut(pin_Mux_B);
  MuxC = new DigitalOut(pin_Mux_C);

  print("Baud rate Nano is %d\n", BaudRateNano);

  RXChannelMux = new BufferedSerial(NC, pin_Mux_Nano_RX, BaudRateNano);
  PinName p[] = {pin_Nano_Rx}; //! pin_Nano_Rx is a list of pin with direct
                               //! connexion to nano and serial support
  for (int i = 0; i < sizeof(p) / sizeof(p[0]); ++i) {
    RXChannels.push_back(new BufferedSerial(NC, p[i], BaudRateNano));
  }
  this->cb = cb;

  flag = mainFlags;
}

void MuxCommunication::run() {
  Thread *t = new Thread();
  t->start(callback(this, &MuxCommunication::threadWorkerMux));
  threads.push_back(t);
  t = new Thread();
  t->start(callback(this, &MuxCommunication::threadWorkerRx0));
  threads.push_back(t);
  t = new Thread();
  t->start(callback(this, &MuxCommunication::threadWorkerRx1));
  threads.push_back(t);
  t = new Thread();
  t->start(callback(this, &MuxCommunication::threadWorkerRx2));
  threads.push_back(t);
}

void MuxCommunication::threadWorkerRx(int i) {
  int linearActuatorSelected;
  int legSelected;
  int mesureInt;
  uint32_t v = 0;
  unsigned char c = 0;
  auto dt = Kernel::Clock::now();
  unsigned char rbuf[100];
  if (i < 0 || (i >= RXChannels.size()))
    return;
  BufferedSerial *rxChannel = RXChannels[i];
  if (rxChannel == nullptr)
    return;
  while (1) {
    if (rxChannel->readable()) {
      int r = rxChannel->read(rbuf, 100);
      for (int l = 0; l < r; ++l) {
        c = rbuf[l];
        v = (v >> 8) | (c << 24);
        if (c == 0x9b) {
          if (check_nano(v, legSelected, linearActuatorSelected, mesureInt)) {
            //  print("twrx %d;%d\n",i,mesureInt);
            if (cb != nullptr)
              cb->value(legSelected, linearActuatorSelected, mesureInt);
            // rxChannel->sync();
          } else {
            // print("read on chan B check wrong ! %x => %x\n", cB, vB);
          }
        }
      }
    }
    ThisThread::yield();
  }
}

void MuxCommunication::threadWorkerMux() {
  int linearActuatorSelected;
  int legSelected;
  int mesureInt;
  uint32_t vA = 0;
  unsigned char cA = 0;
  auto dt = Kernel::Clock::now();
  bool readA = false; // readB = false;
  uint8_t wordAB = 0;
  failed = 0;
  success = 0;
  unsigned char rbuf[200];
  int nNanos = 6 - RXChannels.size();
  while (1) {
    if (RXChannelMux->readable()) {
      int r = RXChannelMux->read(rbuf, sizeof(rbuf));
      for (int l = 0; l < sizeof(rbuf) && readA == false; ++l) {
        cA = rbuf[l];
        vA = (vA >> 8) | (cA << 24);
        if (cA == 0x9b &&
            check_nano(vA, legSelected, linearActuatorSelected, mesureInt)) {
          readA = true;
          if (cb != nullptr)
            cb->value(legSelected, linearActuatorSelected, mesureInt);
          // legs[legSelected]
        }
      }
    }

    if ((readA) || ((Kernel::Clock::now() - dt) > 1ms)) {
      if (readA)
        success += 1;
      else
        failed += 1;
      wordAB = (wordAB + 1) % nNanos;
      MuxA->write(wordAB & 0x1);
      MuxB->write((wordAB >> 1) & 0x1);
      MuxC->write((wordAB >> 2) & 0x1);
      // print("switch chan %d (read: %d)\n",wordAB,readA);
      RXChannelMux->sync();
      readA = false;
      vA = 0;
      dt = Kernel::Clock::now();
    }
  }
}

bool MuxCommunication::check_nano(uint32_t value, int &legSelected,
                                  int &linearActuatorSelected,
                                  int &mesureInt) {
  linearActuatorSelected = -1;
  legSelected = -1;
  struct nanoValue *v = (struct nanoValue *)&value;
  if (v->header == 0x9b) {
    uint32_t pbit = v->value ^ (v->value >> 1);
    pbit = pbit ^ (pbit >> 2);
    pbit = pbit ^ (pbit >> 4);
    pbit = pbit ^ (pbit >> 8);
    pbit = pbit & 0x1;
    if ((v->padding == pbit) &&
        (v->check == get_crc(value & 0xFF, (value >> 8) & 0xFF))) {
      // Vérification de la Leg
      // debug("header and CRC ok %d %d %d\n", v->legId, v->actuatorId,
      // v->value);
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
      // debug("CRC not ok %x %x %d %d %d\n", v->header, v->check, v->legId,
      //      v->actuatorId, v->value);
    }
  }
  return false;
}
