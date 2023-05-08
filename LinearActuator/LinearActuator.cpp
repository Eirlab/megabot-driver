#include "LinearActuator.h"

LinearActuator::LinearActuator(LinearActuatorId_t linear_actuator_position_leg, PinName PWM, PinName DIR1, PinName DIR2,
                               uint16_t position_int_min,
                               uint16_t position_int_max, EventFlags *main_flag) {
    IdLinearActuator = linear_actuator_position_leg;

    pwm = new PwmOut(PWM);
    pwm->period_us(PeriodUsController);
    pwm->write(0);
    dir1 = new DigitalOut(DIR1);
    dir2 = new DigitalOut(DIR2);

    flag = main_flag;

    sens = none;

    timeRampe.reset();

    trigMax = position_int_max;
    trigMin = position_int_min;
    positionInt = (int) ((trigMax - trigMin) / 2);
    resolution = (float) (RangeLinearActuator) / ((float) (trigMax - trigMin));
    positionMm = resolution * float(positionInt-trigMin);

    target = positionMm;
    error = 0.;

}

LinearActuator::~LinearActuator() {
    delete pwm;
    delete dir1;
    delete dir2;
}


float LinearActuator::getPositionMm() {
    positionMm = (float) (RangeLinearActuator) / ((float) (trigMax - trigMin));
    if (positionMm < 0. || positionMm > RangeLinearActuator) {
        return -1; // DANGER
        //FIXME flag emergency
    } else {
        return positionMm;
    }
}

bool LinearActuator::setPositionMm(float target_position) {
    //TODO rampe au démarrage et arrêt d'urgence
    target = target_position;
    error = positionMm - target_position;
// Détermination du sens à adopter
    if (error > 0) {
        sens = forward;
    } else if (error < 0) {
        sens = backward;
    } else {
        sens = none;
    }
    setSens(sens);

// Application de la rampe
//    setPwm(float intensity)

    return false;
}

void LinearActuator::setSens(LinearActuator::sens_t sensTarget) {
    switch (sensTarget) {
        //FIXME sens de la commande
        case forward:
            dir1->write(1);
            dir2->write(0);
            break;
        case backward:
            dir1->write(0);
            dir2->write(1);
            break;
        case none:
            dir1->write(0);
            dir2->write(0);
            break;
    }
}

void LinearActuator::setPwm(float intensity) {
    float cmd;
    if (intensity > 1){
        cmd = 1;
    }
    else if (intensity < 0) {
        cmd = 0;
    }
    else {
        cmd = intensity;
    }
    pwm->write(cmd);
}

void LinearActuator::setPositionInt(uint16_t mesure_int) {
    positionInt = mesure_int;
    positionMm = resolution * (positionInt-trigMin);
}


