#ifndef MEGABOT_DRIVER_PINOUT_H
#define MEGABOT_DRIVER_PINOUT_H
#include "mbed.h"

#define pin_Mux_A                PB_1
#define pin_Mux_B                PB_2
#define pin_Mux_C                PB_3
#define pin_Mux_Nano_RX          PA_10
#define pin_Nano_Rx              PD_2,PA_1,PC_5

#define pin_C1A_PWM                    PB_7 // PWM 4/2
#define pin_C1A_DIR1                    PH_0
#define pin_C1A_DIR2                    PH_1
//#define pin_LegA_baseLeg_trigMinMax     PH_0

#define pin_C1B_PWM          PB_0 // PWM 3/3
#define pin_C1B_DIR1         PC_2
#define pin_C1B_DIR2         PC_3
//#define pin_LegA_middleLeg_trigMinMax   PC_1

#define pin_C2A_PWM             PA_15 // PWM 2/1
#define pin_C2A_DIR1            PA_14
#define pin_C2A_DIR2            PA_13
//#define pin_LegA_endLeg_trigMinMax      PC_12

#define pin_C2B_PWM            PC_9 // PWM 8/4
#define pin_C2B_DIR1           PC_8
#define pin_C2B_DIR2           PC_6
//#define pin_LegB_baseLeg_trigMinMax     PB_8

#define pin_C3A_PWM          PB_4    // PWM 3/1
#define pin_C3A_DIR1         PB_5
#define pin_C3A_DIR2         PB_6
//#define pin_LegB_middleLeg_trigMinMax   PB_10

#define pin_C3B_PWM             PA_11  // PWM 1/4
#define pin_C3B_DIR1            PB_14
#define pin_C3B_DIR2            PB_13
//#define pin_LegBb_endLeg_trigMinMax     PB_12

#endif //MEGABOT_DRIVER_PINOUT_H
