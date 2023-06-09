#include "mbed.h"
#include "Leg.h"
#include "LinearActuator.h"
#include "MuxCommunication.h"
#include "MasterCommunication.h"
#include "PinOut.h"
#include "GlobalConfig.h"


/* ************************************************
 *                  Objects
 * ************************************************/
EventFlags Flags;
BufferedSerial serialMaster(USBTX, USBRX, BaudRateMaster);

LinearActuator baseLegA(baseLeg, pin_LegA_baseLeg_PWM, pin_LegA_baseLeg_DIR1, pin_LegA_baseLeg_DIR2,
                        LegA_baseLeg_Pos_Min,
                        LegA_baseLeg_Pos_Max, &Flags);

LinearActuator middleLegA(middleLeg, pin_LegA_middleLeg_PWM, pin_LegA_middleLeg_DIR1, pin_LegA_middleLeg_DIR2,
                          LegA_middleLeg_Pos_Min, LegA_middleLeg_Pos_Max, &Flags);

LinearActuator endLegA(endLeg, pin_LegA_endLeg_PWM, pin_LegA_endLeg_DIR1, pin_LegA_endLeg_DIR2, LegA_endLeg_Pos_Min,
                       LegA_endLeg_Pos_Max, &Flags);

LinearActuator baseLegB(baseLeg, pin_LegB_baseLeg_PWM, pin_LegB_baseLeg_DIR1, pin_LegB_baseLeg_DIR2,
                        LegB_baseLeg_Pos_Min,
                        LegB_baseLeg_Pos_Max, &Flags);

LinearActuator middleLegB(middleLeg, pin_LegB_middleLeg_PWM, pin_LegB_middleLeg_DIR1, pin_LegB_middleLeg_DIR2,
                          LegB_middleLeg_Pos_Min, LegB_middleLeg_Pos_Max, &Flags);

LinearActuator endLegB(endLeg, pin_LegB_endLeg_PWM, pin_LegB_endLeg_DIR1, pin_LegB_endLeg_DIR2, LegB_endLeg_Pos_Min,
                       LegB_endLeg_Pos_Max, &Flags);

//TODO: Ici on met à jour l'id de la Leg et on va vérifier dans GlobalConfig que les flags soient bien configurés
Leg legA(leg1, &baseLegA, &middleLegA, &endLegA);
Leg legB(leg2, &baseLegB, &middleLegB, &endLegB);


// MasterCommunication masterCom(&serialMaster, &Flags, &legA, &legB);
MuxCommunication muxCom(pin_Mux_PinOut_A, pin_Mux_PinOut_B, pin_Mux_PinOut_TX_LEGA, pin_Mux_PinOut_TX_LEGB, &Flags,
                        &legA, &legB);


/* ************************************************
 *         MAIN - LinearActuator thread
 * ************************************************/
// void tickerWorker(){

// }
int main() {
    char buffer[255];
    
    muxCom.run();
    while (true) {
        int mesAbase = legA.linear_actuator_baseLeg->positionInt;
        int mesAmiddle = legA.linear_actuator_middleLeg->positionInt;
        int mesAend = legA.linear_actuator_endLeg->positionInt;
        int mesBbase = legB.linear_actuator_baseLeg->positionInt;
        int mesBmiddle = legB.linear_actuator_middleLeg->positionInt;
        int mesBend = legB.linear_actuator_endLeg->positionInt;
        
        uint8_t len = sprintf(buffer, "> LegA : %d %d %d \n> LegB : %d %d %d \n\n",
        mesAbase, mesAmiddle, mesAend,
        mesBbase, mesBmiddle, mesBend );
        serialMaster.write(buffer, len);

        ThisThread::sleep_for(1s);
    }
}
