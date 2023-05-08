#ifndef MEGABOT_DRIVER_COMLINEARACTUATOR_H
#define MEGABOT_DRIVER_COMLINEARACTUATOR_H

#include "mbed.h"
#include "Leg.h"

class ComLinearActuator {
public:
    ComLinearActuator(Leg *leg1, Leg *leg2);
    void updateValue(uint8_t id_leg, LinearActuatorLeg_t linear_actuator, uint32_t mesureInt);
private:
    Leg* Leg1;
    Leg* Leg2;
};


#endif //MEGABOT_DRIVER_COMLINEARACTUATOR_H
