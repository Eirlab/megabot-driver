#ifndef MEGABOT_DRIVER_GLOBALCONFIG_H
#define MEGABOT_DRIVER_GLOBALCONFIG_H
#include "mbed.h"

/* ************************************************
 *          Configuration Nécessaire
 * ************************************************/
// TODO à configurer
//#define CONTROLER12
#ifdef CONTROLER12
#define BaudRateNano            230400
#else
#define BaudRateNano            115200 //pour les legs 1&2: 230400
#endif
#define BaudRateMaster          115200
#define MaxCharactersFloat      8           // caractères maximums reçu sans délimiteur final (venant du master) avant de retourner une erreur)

#define PeriodUsController      40          // 25 kHz

#define RangeLinearActuator     200         // 20 cm, 200 mm

/* ************************************************
 *               Min Max LinearActuator
 * ************************************************/
#define LegA_baseLeg_Pos_Min    0
#define LegA_baseLeg_Pos_Max    1023

#define LegA_middleLeg_Pos_Min  0
#define LegA_middleLeg_Pos_Max  1023

#define LegA_endLeg_Pos_Min      0
#define LegA_endLeg_Pos_Max     1023

#define LegB_baseLeg_Pos_Min    0
#define LegB_baseLeg_Pos_Max    1023

#define LegB_middleLeg_Pos_Min  0
#define LegB_middleLeg_Pos_Max  1023

#define LegB_endLeg_Pos_Min     0
#define LegB_endLeg_Pos_Max     1023



/* ************************************************
 *                   Flags
 * ************************************************/
#define FlagRunCmd              0x10
#define Flag_LegA_1             (1 << 0x1)
#define Flag_LegA_2             (1 << 0x2)
#define Flag_LegA_3             (1 << 0x3)
#define Flag_LegB_1             (1 << 0x4)
#define Flag_LegB_2             (1 << 0x5)
#define Flag_LegB_3             (1 << 0x6)
#define Flag_LegA               (Flag_LegA_1) | (Flag_LegA_2) | (Flag_LegA_3)
#define Flag_LegB               (Flag_LegB_1) | (Flag_LegB_2) | (Flag_LegB_3)


#endif //MEGABOT_DRIVER_GLOBALCONFIG_H