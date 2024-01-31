#ifndef MEGABOT_DRIVER_MUXCOMMUNICATION_H
#define MEGABOT_DRIVER_MUXCOMMUNICATION_H

#include "GlobalConfig.h"
#include "PinNames.h"
#include "mbed.h"
#include <vector>
#include "crc.h"

class MuxComCallback {
public:
  virtual ~MuxComCallback() {}
  virtual void value(int leg, int actuator, int value) = 0;
};

class MuxCommunication {
public:
  /**
   * Initialisation des protocoles de communication pour recevoir l'élongation
   * de chacun des LinearActurator. Envoie ces valeurs à la classe ?
   *
   * @param channelA  : PinName       : pour le channel A du multiplexeur
   * @param channelB  : PinName       : pour le channel B du multiplexeur
   * @param TXLeg1    : PinName       : correspondant au TX de la Leg1
   * sélectionné par AB
   * @param TXLeg2    : PinName       : correspondant au TX de la Leg2
   * sélectionné par AB
   * @param mainFlags : EventFlags    : pointeur vers le Flag d'arrêt d'urgence
   */
  MuxCommunication(EventFlags *mainFlags, MuxComCallback *cb);

  void run();

  /**
   * @brief Comportement classique dans le Thread : attente d'un message soit
   * disponible sur le Port sélectionner
   */
  void threadWorkerMux();
  void threadWorkerRx(int i);

  void threadWorkerRx0(){threadWorkerRx(0);}
  void threadWorkerRx1(){threadWorkerRx(1);}
  void threadWorkerRx2(){threadWorkerRx(2);}
unsigned int failed;
unsigned int success;
private:
  DigitalOut *MuxA;
  DigitalOut *MuxB;
  DigitalOut *MuxC;
  BufferedSerial *RXChannelMux;

  std::vector<BufferedSerial *> RXChannels;

  MuxComCallback *cb;

  EventFlags *flag;

  vector<Thread *> threads;

  // Communication tools
  struct nanoValue {
    uint32_t value : 10;
    uint32_t actuatorId : 2;
    uint32_t legId : 3;
    uint32_t padding : 1;
    uint32_t check : 8;
    uint32_t header : 8;
  };

  bool check_nano(uint32_t value, int &legSelected, int &linearActuatorSelected,
                  uint16_t &mesureInt);
};

#endif // MEGABOT_DRIVER_MUXCOMMUNICATION_H
