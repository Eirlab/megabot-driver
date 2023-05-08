#ifndef MEGABOT_DRIVER_LINEARACTUATOR_H
#define MEGABOT_DRIVER_LINEARACTUATOR_H

#include "mbed.h"
#include "GlobalConfig.h"

<<<<<<< Updated upstream
enum LinearActuatorId_t{
=======
enum LinearActuatorLeg_t{
>>>>>>> Stashed changes
    baseLeg     = 1,
    middleLeg   = 2,
    endLeg      = 3
};

class LinearActuator {
<<<<<<< Updated upstream
    //TODO Documentation
private:
    enum sens_t{
        none = 0,
        forward = 1,
        backward = -1
    };
    sens_t sens;
public:
    LinearActuator(LinearActuatorId_t linear_actuator_position_leg, PinName PWM, PinName DIR1, PinName DIR2,
                   uint16_t position_int_min, uint16_t position_int_max, EventFlags *main_flag);
=======
    LinearActuator(LinearActuatorLeg_t id, PinName PWM, PinName DIR1, PinName DIR2);
>>>>>>> Stashed changes
    ~LinearActuator();

    float getPositionMm();
    bool setPositionMm(float target);

    uint16_t positionInt; // lecture de la position du vérin par rapport à communication sur la Nano
    void setPositionInt(uint16_t mesure_int);

    /**
     * Configure les pins DIR1 et DIR2 pour que le sens soit correct
     * @param sensTarget : sens_t
     */
    void setSens(sens_t sensTarget);
private:
    LinearActuatorId_t IdLinearActuator;
    PwmOut* pwm;
    DigitalOut* dir1;
    DigitalOut* dir2;

    EventFlags* flag;

    float positionMm;
    float resolution; // à ne pas modifier
    uint16_t trigMin;
    uint16_t trigMax;

    float target, error;




    /**
     * Applique le bon rapport à la PWM en prenant en compte la rampe de démarrage
     * @param intensity : (float)
     */
    void setPwm(float intensity);
    Timer timeRampe;
};


#endif //MEGABOT_DRIVER_LINEARACTUATOR_H
