#ifndef MEGABOT_DRIVER_LEG_H
#define MEGABOT_DRIVER_LEG_H

#include "LinearActuator.h"


enum LegId_t {
    leg1 = 1,
    leg2 = 2,
    leg3 = 3,
    leg4 = 4
};


class Leg {
    //TODO Documentation classe
public:
/**
 * @brief Création d'une patte constituant 3 vérins
 * @param leg_id : (LegId_t) identifiant de la patte (leg1, leg2, leg3 ou leg4)
 * @param base_leg : (LinearActuator *) LinearActuator étant le baseLeg
 * @param middle_leg : (LinearActuator *) LinearActuator étant le middleLeg
 * @param end_leg : (LinearActuator *) LinearActuator étant le endLeg
 */
    Leg(LegId_t leg_id, LinearActuator *base_leg, LinearActuator *middle_leg, LinearActuator *end_leg);

    void updateValuePositionInt(LinearActuatorId_t linear_actuator_select, uint16_t mesureInt) const;
    LegId_t getId() const;

    LinearActuator *linear_actuator_baseLeg;    //< Vérin présent dans le cadre du MégaBot, communément numéroté 1
    LinearActuator *linear_actuator_middleLeg;  //< Vérin en milieu de patte du MégaBot, communément numéroté 2
    LinearActuator *linear_actuator_endLeg;     //< Vérin en bout de patte du MégaBot, communément numéroté 3
private:
    LegId_t legId;
};




#endif //MEGABOT_DRIVER_LEG_H
