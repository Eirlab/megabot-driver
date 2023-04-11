#ifndef MEGABOT_DRIVER_GLOBALCONFIG_H
#define MEGABOT_DRIVER_GLOBALCONFIG_H

/* ************************************************
 *                   Flags
 * ************************************************/
#define FlagRunCmd              0x47


/* ************************************************
 *               Communication
 * ************************************************/

#define BaudRateNano            115200
#define BaudRateMaster          9600
#define MaxCharactersFloat      8

#define PeriodUsController      40          // 25 kHz

#define RangeLinearActuator     200         // 20 cm, 200 mm

#define event_TimeOut_Tx        0x10

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



#endif //MEGABOT_DRIVER_GLOBALCONFIG_H
