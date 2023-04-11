#include "MuxCommunication.h"
#include "MasterCommunication.h"

MuxCommunication::MuxCommunication(PinName channelA, PinName channelB, PinName TX_legA, PinName TX_legB,
                                   EventFlags *emergency,
                                   Leg *leg_A, Leg *leg_B) {
    A = new DigitalOut(channelA);
    B = new DigitalOut(channelB);
    TXLegA = new BufferedSerial(NC, TX_legA, BaudRateNano);
    TXLegB = new BufferedSerial(NC, TX_legB, BaudRateNano);

    legA = leg_A;
    legB = leg_B;

    emergencyFlag = emergency;
//    timeoutFlag = new EventFlags ();


    linearActuatorSelected = baseLeg;
    legSelected = legA->getId();
    vA = 0;
    cA = 0;
    vB = 0;
    cB = 0;
    wordAB = 0;
}


void MuxCommunication::threadWorker() {
    if (TXLegA->readable() && TXLegB->readable()) {
        emergencyFlag->set(event_TimeOut_Tx);

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
            };
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


void MuxCommunication::update_crc(unsigned char &crc) {
    for (int j = 0; j < 8; j++) {
        if (crc & 1)
            crc ^= CRC7_POLY;
        crc >>= 1;
    }
}

unsigned char MuxCommunication::get_crc(unsigned char v1, unsigned char v2) {
    unsigned char crc = 0;
    crc ^= v1;
    update_crc(crc);
    crc ^= v2;
    update_crc(crc);
    return crc;
}

Callback<void()> MuxCommunication::main() {

    while (true){
        threadWorker();
    }
}

