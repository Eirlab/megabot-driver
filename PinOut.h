#ifndef MEGABOT_DRIVER_PINOUT_H
#define MEGABOT_DRIVER_PINOUT_H
#include "mbed.h"

#define pin_Mux_PinOut_A        PB_1
#define pin_Mux_PinOut_B        PB_2
#define pin_Mux_PinOut_TX_LEGA  PA_10
#define pin_Mux_PinOut_TX_LEGB  PC_5

#define pin_LegA_baseLeg_PWM            PB_7
#define pin_LegA_baseLeg_DIR1           PC_14
#define pin_LegA_baseLeg_DIR2           PC_15
#define pin_LegA_baseLeg_trigMinMax     PH_0

#define pin_LegA_middleLeg_PWM          PB_0
#define pin_LegA_middleLeg_DIR1         PC_2
#define pin_LegA_middleLeg_DIR2         PC_3
#define pin_LegA_middleLeg_trigMinMax   PC_1

#define pin_LegA_endLeg_PWM             PA_15
#define pin_LegA_endLeg_DIR1            PA_14
#define pin_LegA_endLeg_DIR2            PA_13
#define pin_LegA_endLeg_trigMinMax      PC_12

#define pin_LegB_baseLeg_PWM            PC_9
#define pin_LegB_baseLeg_DIR1           PC_8
#define pin_LegB_baseLeg_DIR2           PC_6
#define pin_LegB_baseLeg_trigMinMax     PB_8

#define pin_LegB_middleLeg_PWM          PB_4
#define pin_LegB_middleLeg_DIR1         PB_5
#define pin_LegB_middleLeg_DIR2         PB_3
#define pin_LegB_middleLeg_trigMinMax   PB_10

#define pin_LegB_endLeg_PWM             PB_15
#define pin_LegB_endLeg_DIR1            PB_14
#define pin_LegB_endLeg_DIR2            PB_13
#define pin_LegBb_endLeg_trigMinMax      PB_12


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
