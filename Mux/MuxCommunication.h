#ifndef MEGABOT_DRIVER_MUXCOMMUNICATION_H
#define MEGABOT_DRIVER_MUXCOMMUNICATION_H

<<<<<<< Updated upstream
#include <MasterCommunication.h>
#include "mbed.h"
#include "Leg.h"
#include "GlobalConfig.h"
=======
#include "mbed.h"
#include "ComLinearActuator.h"


#define BaudRate 115200
>>>>>>> Stashed changes

const unsigned char CRC7_POLY = 0x91;

class MuxCommunication {
<<<<<<< Updated upstream
public:
=======
    //FIXME classe de transfert pour les données
>>>>>>> Stashed changes
    /**
     * Initialisation des protocoles de communication pour recevoir l'élongation de chacun des LinearActurator.
     * Envoie ces valeurs à la classe ?
     *
     * @param channelA  : PinName       : pour le channel A du multiplexeur
     * @param channelB  : PinName       : pour le channel A du multiplexeur
     * @param TXLeg1    : PinName       : correspondant au TX de la Leg1 sélectionné par AB
     * @param TXLeg2    : PinName       : correspondant au TX de la Leg2 sélectionné par AB
<<<<<<< Updated upstream
     * @param mainFlags : EventFlags    : pointeur vers le Flag d'arrêt d'urgence
     */
    MuxCommunication(PinName channelA, PinName channelB, PinName TX_legA, PinName TX_legB, EventFlags *mainFlags,
                     Leg *leg_A, Leg *leg_B);

    /**
     * @brief Comportement classique dans le Thread : attente d'un message soit disponible sur le Port sélectionner
     */
    void threadWorker();

    /**
     * @brief Equivalent du main pour le Thread associé à cette classe
     * utile pour faire fonctionner de manière 'classique' le threadWorker et de pouvoir y ajouter des sécurités
     */
    Callback<void()> main();

private:
    DigitalOut *A;
    DigitalOut *B;
    BufferedSerial *TXLegA;
    BufferedSerial *TXLegB;

    Leg *legA;
    Leg *legB;

    uint8_t wordAB;
    LinearActuatorId_t linearActuatorSelected;
    LegId_t legSelected;

    EventFlags *flag;


// Communication tools
    uint32_t vA;
    unsigned char cA;
    uint32_t vB;
    unsigned char cB;
    struct nanoValue {
        uint32_t value: 10;
        uint32_t actuatorId: 2;
        uint32_t legId: 3;
        uint32_t padding: 1;
        uint32_t check: 8;
        uint32_t header: 8;
    };

    static void update_crc(unsigned char &crc);

    static unsigned char get_crc(unsigned char v1, unsigned char v2);

    bool check_nano(uint32_t value, uint16_t &mesureInt);
=======
     * @param emergency : EventFlags    : pointeur vers le Flag d'arrêt d'urgence
     */
    MuxCommunication(PinName channelA, PinName channelB, PinName TXLeg1, PinName TXLeg2, EventFlags* emergency, ComLinearActuator* interfaceLA);

    /**
     * Le Thread attend qu'un message soit disponible sur le Port sélectionner
     */
    void threadWorker();

private:
    DigitalOut* A;
    DigitalOut* B;
    BufferedSerial* TXLeg1;
    BufferedSerial* TXLeg2;

    uint8_t wordAB;
    LinearActuatorLeg_t linearActuatorSelected;
    Leg_t legSelected;

    EventFlags* emergencyFlag;

    ComLinearActuator* interfaceLA;


// Communication tools
    struct nanoValue{
        uint32_t            value:10;
        LinearActuatorLeg_t actuatorId:2;
        Leg_t               legId:3;

        uint32_t check:8;
        uint32_t header:8;
    };
    static void update_crc(unsigned char &crc);
    static unsigned char get_crc(unsigned char v1, unsigned char v2);
    static bool check_nano(uint32_t value, Leg_t &legId, LinearActuatorLeg_t &linear_actuator_id, uint32_t &mesureInt);
>>>>>>> Stashed changes
};


#endif //MEGABOT_DRIVER_MUXCOMMUNICATION_H
