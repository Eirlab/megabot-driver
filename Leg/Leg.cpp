#include "Leg.h"

Leg::Leg(LegId_t leg_id, LinearActuator *base_leg, LinearActuator *middle_leg, LinearActuator *end_leg) {
    legId = leg_id;

    linear_actuator_baseLeg = base_leg;
    linear_actuator_middleLeg = middle_leg;
    linear_actuator_endLeg = end_leg;
}

void Leg::updateValuePositionInt(LinearActuatorId_t linear_actuator_select, uint16_t mesureInt) const {
    switch (linear_actuator_select) {
        case baseLeg:
            linear_actuator_baseLeg->positionInt = mesureInt;
            break;
        case middleLeg:
            linear_actuator_middleLeg->positionInt = mesureInt;
            break;
        case endLeg:
            linear_actuator_endLeg->positionInt = mesureInt;
            break;
    }
}

LegId_t Leg::getId() const {
    return legId;
}
