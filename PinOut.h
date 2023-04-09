#ifndef MEGABOT_DRIVER_PINOUT_H
#define MEGABOT_DRIVER_PINOUT_H
#include "mbed.h"

#define Mux_PinOut_A        PB_1
#define Mux_PinOut_B        PB_2
#define Mux_PinOut_TX_LEG1  PA_10
#define Mux_PinOut_TX_LEG2  PC_5

DigitalOut(Mux_PinOut_A);
DigitalOut(Mux_PinOut_B);

#define Leg1_baseLeg_PWM    PB_7
#define Leg1_baseLeg_DIR1   PC_14
#define Leg1_baseLeg_DIR2   PC_15

#define Leg1_middleLeg_PWM  PB_0
#define Leg1_middleLeg_DIR1 PC_2
#define Leg1_middleLeg_DIR2 PC_3

#define Leg1_endLeg_PWM     PA_15
#define Leg1_endLeg_DIR1    PA_14
#define Leg1_endLeg_DIR2    PA_13

#define Leg2_baseLeg_PWM    PC_9
#define Leg2_baseLeg_DIR1   PC_8
#define Leg2_baseLeg_DIR2   PC_6

#define Leg2_middleLeg_PWM  PB_4
#define Leg2_middleLeg_DIR1 PB_5
#define Leg2_middleLeg_DIR2 PB_3

#define Leg2_endLeg_PWM     PB_15
#define Leg2_endLeg_DIR1    PB_14
#define Leg2_endLeg_DIR2    PB_13






struct PinOut_LinearActuator_t {
    PinName PWM;
    PinName DIR1;
    PinName DIR2;
};

struct PinOut_Mux_t {
    PinName A;
    PinName B;
    PinName TX_Leg1;
    PinName TX_Leg2;
};

#endif //MEGABOT_DRIVER_PINOUT_H
