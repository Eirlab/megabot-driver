#ifndef MEGABOT_DRIVER_LEG_H
#define MEGABOT_DRIVER_LEG_H

#include "LinearActuator.h"


enum Leg_t {
    leg1 = 1,
    leg2 = 2,
    leg3 = 3,
    leg4 = 4
};


class Leg {
public:
    Leg(Leg_t leg_id, LinearActuator *base_leg, LinearActuator *middle_leg, LinearActuator *end_leg);

    void updateValuePositionInt(LinearActuatorLeg_t linear_actuator_select, uint16_t mesureInt) const;
    Leg_t getId() const;

    Leg_t legId;

    LinearActuator *linear_actuator_baseLeg;
    LinearActuator *linear_actuator_middleLeg;
    LinearActuator *linear_actuator_endLeg;


};

Leg* getLeg(Leg_t leg_select);


#endif //MEGABOT_DRIVER_LEG_H
