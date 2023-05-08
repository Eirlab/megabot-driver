#include "MuxCommunication.h"
<<<<<<< Updated upstream
#include "MasterCommunication.h"

MuxCommunication::MuxCommunication(PinName channelA, PinName channelB, PinName TX_legA, PinName TX_legB,
                                   EventFlags *mainFlags,
                                   Leg *leg_A, Leg *leg_B) {
    A = new DigitalOut(channelA);
    B = new DigitalOut(channelB);
    TXLegA = new BufferedSerial(NC, TX_legA, BaudRateNano);
    TXLegB = new BufferedSerial(NC, TX_legB, BaudRateNano);

    legA = leg_A;
    legB = leg_B;

    flag = mainFlags;



    linearActuatorSelected = baseLeg;
    legSelected = legA->getId();
    vA = 0;
    cA = 0;
    vB = 0;
    cB = 0;
=======

MuxCommunication::MuxCommunication(PinName channelA, PinName channelB, PinName TX_leg1, PinName TX_leg2,
                                   EventFlags *emergency, ComLinearActuator *interface_linear_actuator) {
    A = new DigitalOut(channelA);
    B = new DigitalOut(channelB);
    TXLeg1 = new BufferedSerial(TX_leg1, NC, BaudRate);
    TXLeg2 = new BufferedSerial(TX_leg2, NC, BaudRate);

    interfaceLA = interface_linear_actuator;

    emergencyFlag = emergency;

>>>>>>> Stashed changes
    wordAB = 0;
}


void MuxCommunication::threadWorker() {
<<<<<<< Updated upstream
    if (TXLegA->readable() && TXLegB->readable()) {
        flag->set(event_TimeOut_Tx);

        uint16_t mesureInt;

        // Read TXLegA
        TXLegA->read(&cA, sizeof(cA));

        vA = (vA >> 8) | (cA << 24);

        if (check_nano(vA, mesureInt)) {
            legA->updateValuePositionInt(linearActuatorSelected, mesureInt);
        }


        // Read TXLegB
        TXLegB->read(&cB, sizeof(cB));
        vB = (vB >> 8) | (cB << 24);
        if (check_nano(vB, mesureInt)) {
            legB->updateValuePositionInt(linearActuatorSelected, mesureInt);
=======
    if (TXLeg1->readable() && TXLeg2->readable()) {
        std::string str;
        char c1, c2;
        uint32_t v1, v2;
        uint32_t mesureInt;

        // Read TXLeg1
        TXLeg1->read(&c1, sizeof(c1));

        v1 = (v1 >> 8) | (c1 << 24);

        if (check_nano(v1, legSelected, linearActuatorSelected, mesureInt)) {
            interfaceLA->updateValue(legSelected, linearActuatorSelected, mesureInt);
        }


        // Read TXLeg2
        TXLeg2->read(&c2, sizeof(c2));
        v2 = (v2 >> 8) | (c2 << 24);
        if (check_nano(v2, legSelected, linearActuatorSelected, mesureInt)) {
            interfaceLA->updateValue(legSelected, linearActuatorSelected, mesureInt);
>>>>>>> Stashed changes
        }


        switch (wordAB) {
            case 0:
                A->write(0);
                B->write(1);
                break;
            case 1:
                A->write(1);
                B->write(0);
                break;
            case 2:
                A->write(0);
                B->write(0);
                break;
            default:
                A->write(0);
                B->write(0);
        }

        wordAB = A->read() << 1 | B->read();
    }
}


<<<<<<< Updated upstream
bool MuxCommunication::check_nano(uint32_t value, uint16_t &mesureInt) {
    struct nanoValue *v = (struct nanoValue *) &value;
    if (v->header == 0x55) {
        if (v->check == get_crc(value & 0xFF, (value >> 8) & 0xFF)) {
            // Vérification de la Leg
            switch (v->legId) {
                case 1:
                    legSelected = leg1;
                    break;
                case 2:
                    legSelected = leg2;
                    break;
                case 3:
                    legSelected = leg3;
                    break;
                case 4:
                    legSelected = leg4;
                    break;
                default:
                    return false;
            }
            // Vérification de la LinearActuator
            switch (v->actuatorId) {
                case 1:
                    linearActuatorSelected = baseLeg;
                    break;
                case 2:
                    linearActuatorSelected = middleLeg;
                    break;
                case 3:
                    linearActuatorSelected = endLeg;
                    break;
                default:
                    return false;
            };
            if (v->value > 1023 || v->value < 0) {
                return false;
            } else {
                mesureInt = v->value;
            }
            return true;
        }
    }
    return false;
}


=======
>>>>>>> Stashed changes
void MuxCommunication::update_crc(unsigned char &crc) {
    for (int j = 0; j < 8; j++) {
        if (crc & 1)
            crc ^= CRC7_POLY;
        crc >>= 1;
    }
}

<<<<<<< Updated upstream
=======
bool MuxCommunication::check_nano(uint32_t value, Leg_t &legId, LinearActuatorLeg_t &linear_actuator_id,
                                  uint32_t &mesureInt) {
    struct nanoValue *v = (struct nanoValue *) &value;
    if (v->header == 0x55) {
        if (v->check == get_crc(value & 0xFF, (value >> 8) & 0xFF)) {
            legId = v->legId;
            linear_actuator_id = v->actuatorId;
            mesureInt = v->value;
            return true;
        }
    }
    return false;
}

>>>>>>> Stashed changes
unsigned char MuxCommunication::get_crc(unsigned char v1, unsigned char v2) {
    unsigned char crc = 0;
    crc ^= v1;
    update_crc(crc);
    crc ^= v2;
    update_crc(crc);
    return crc;
}

<<<<<<< Updated upstream
Callback<void()> MuxCommunication::main() {

    while (true){
        threadWorker();
    }
}

=======
>>>>>>> Stashed changes
