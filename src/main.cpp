#include "mbed.h"
#include "Leg.h"
#include "LinearActuator.h"
#include "MuxCommunication.h"
#include "MasterCommunication.h"
#include "PinOut.h"
#include "GlobalConfig.h"

/* ************************************************
 *               Config Flag Thread
 * ************************************************/

Thread communication_master;    // Communication avec le controller
Thread communication_nano;      // Communication avec les Nano
Thread control_linearActurator; // Contrôle des vérins

EventFlags emergency;   // timout pour la reception de donnée des Nano

BufferedSerial serialMaster(USBTX, USBRX, BaudRateMaster);
//DigitalOut led(LED1);
/* ************************************************
 *                  Object
 * ************************************************/

LinearActuator baseLegA(baseLeg, pin_LegA_baseLeg_PWM, pin_LegA_baseLeg_DIR1, pin_LegA_baseLeg_DIR2,
                        LegA_baseLeg_Pos_Min,
                        LegA_baseLeg_Pos_Max, &emergency);

LinearActuator middleLegA(middleLeg, pin_LegA_middleLeg_PWM, pin_LegA_middleLeg_DIR1, pin_LegA_middleLeg_DIR2,
                          LegA_middleLeg_Pos_Min, LegA_middleLeg_Pos_Max, &emergency);

LinearActuator endLegA(endLeg, pin_LegA_endLeg_PWM, pin_LegA_endLeg_DIR1, pin_LegA_endLeg_DIR2, LegA_endLeg_Pos_Min,
                       LegA_endLeg_Pos_Max, &emergency);

LinearActuator baseLegB(baseLeg, pin_LegB_baseLeg_PWM, pin_LegB_baseLeg_DIR1, pin_LegB_baseLeg_DIR2,
                        LegB_baseLeg_Pos_Min,
                        LegB_baseLeg_Pos_Max, &emergency);

LinearActuator middleLegB(middleLeg, pin_LegB_middleLeg_PWM, pin_LegB_middleLeg_DIR1, pin_LegB_middleLeg_DIR2,
                          LegB_middleLeg_Pos_Min, LegB_middleLeg_Pos_Max, &emergency);

LinearActuator endLegB(endLeg, pin_LegB_endLeg_PWM, pin_LegB_endLeg_DIR1, pin_LegB_endLeg_DIR2, LegB_endLeg_Pos_Min,
                       LegB_endLeg_Pos_Max, &emergency);

//TODO Ici on met à jour l'id de la Leg
Leg legA(leg1, &baseLegA, &middleLegA, &endLegA);
Leg legB(leg2, &baseLegB, &middleLegB, &endLegB);


MasterCommunication masterCom(&serialMaster, &emergency, &legA, &legB);
MuxCommunication muxCom(pin_Mux_PinOut_A, pin_Mux_PinOut_B, pin_Mux_PinOut_TX_LEGA, pin_Mux_PinOut_TX_LEGB, &emergency, &legA, &legB);


/* ************************************************
 *          Config Flag Thread fonctions
 * ************************************************/

/**
 * @brief Fonction s'apparentant à un main pour le contrôle des vérins
 *
 * Prise en compte du flag emergency
 */
void Main_Thread_LinearActuator(){
//TODO Flag emergency à mettre en place
    while (true){

    }
}

int main(){
    communication_master.start(masterCom.main());
    communication_nano.start(muxCom.main());
    control_linearActurator.start(Main_Thread_LinearActuator);

    while (true) {

    }
}