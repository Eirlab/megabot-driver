#include "MasterCommunication.h"

void MasterCommunication::threadWorker() {
    char c;
    while (true) {
        if (serial->readable()) {
            serial->read(&c, 1);
            int8_t parserCount = parseCmd(c);
            if (parserCount == 1) {
                sendCheck(true);
            } else if (parserCount == -1) { // il y a une erreur
                sendCheck(false);
            }
        }
    }
}

int8_t MasterCommunication::parseCmd(char c) {
    static bool switchArg = false;
    static int8_t argIndex = 0;
    static int8_t countFloatCharacters = 0;

    if (argIndex == 3) {
        argIndex = 0;
    }
    if (argIndex < 2) {
        switchArg = true; // on lit le numéro de la patte ou du vérin
    }

    if (switchArg == true) { // lecture de leg et linear actuator
        if (argIndex == 0) {
            switch (c) {
                case '1':
                    legSelected = leg1;
                    break;
                case '2':
                    legSelected = leg2;
                    break;
                case '3':
                    legSelected = leg3;
                    break;
                case '4':
                    legSelected = leg4;
                    break;
                default:
                    return -1; // problème sur la lecture de l'id de la patte

            }
        } else if (argIndex == 1) {
            switch (c) {
                case '1':
                    linearActuatorSelected = baseLeg;
                    break;
                case '2':
                    linearActuatorSelected = middleLeg;
                    break;
                case '3':
                    linearActuatorSelected = endLeg;
                    break;
                default:
                    return -1; // problème sur la lecture de l'id du vérin
            }
        }
        argIndex++;
        switchArg = false;
    }
    else { // remplissage du buffer pour la lecture de la target flottante
        if (argIndex == 2) {
            cmdStr.push_back(c);
            countFloatCharacters++;
        }
    }

    // fin de la commande
    if (c == '~') {
        cmdFloat = stof(cmdStr);

        cmdStr.clear();
        argIndex = 0;
        countFloatCharacters = 0;
        return 1;
    }
    else if (countFloatCharacters > MaxCharactersFloat){ // il manque le délimiteur final et on a reçu trop de commandes
        argIndex = 0;
        return -1;
    }

    if (argIndex < 2){ // l'interprétation est toujours en cours
        return 0;
    }
}

bool MasterCommunication::securityTarget(float targetDesired) {
    return false;
}

MasterCommunication::MasterCommunication(BufferedSerial *master, EventFlags *emergency, Leg *leg_A, Leg *leg_B) {
    serial = master;
    emergencyFlag = emergency;
    legA = leg_A;
    legB = leg_B;
    cmdFloat = 150.0;

    linearActuatorSelected = baseLeg;
    legSelected = legA->getId();
}

void MasterCommunication::sendMesure(Leg_t leg_selected, LinearActuatorLeg_t linear_actuator_selected, float mesure) {
    char buffer[10]; // 2 caractères pour la leg et le LinearActuator, 6 caractères pour la target (XXX,XX) et 1 caractère de fin (~) et '\0'
    size_t len = snprintf(buffer, sizeof(buffer), "%d%d%.2f~", leg_selected, linear_actuator_selected, mesure);
    serial->write(buffer, len);
}

void MasterCommunication::sendCheck(bool commandCheck) {
    if (commandCheck) {
        char buffer[10];
        size_t len = snprintf(buffer, sizeof(buffer),"%d%d%.2f~", legSelected, linearActuatorSelected, cmdFloat);
        serial->write(buffer, len);
//        emergencyFlag->set(FlagRunCmd);
    } else {
        const char *buffer = "000.00~";
        serial->write(buffer, strlen(buffer));
    }
}

Callback<void()> MasterCommunication::main() {

    while(true){
        threadWorker();
    }
}
