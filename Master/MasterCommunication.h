#ifndef MEGABOT_DRIVER_MASTERCOMMUNICATION_H
#define MEGABOT_DRIVER_MASTERCOMMUNICATION_H

#include "mbed.h"
#include "Leg.h"
#include "LinearActuator.h"
#include "GlobalConfig.h"

class MasterCommunication {
    //TODO Documentation
public:
    MasterCommunication(BufferedSerial *master, EventFlags *emergency, Leg *leg_A, Leg *leg_B);
    /**
     * @brief Fonctionnement de la communication dans le Thread communication_master
     * Cette fonction s'apparente à la fonction `main()` dans un *main.cpp*.
     * Elle va attendre qu'un caractère soit reçu du Master,
     * puis appeler la fonction `parseCmd` qui va interpréter les caractère 1 à 1
     * pour ensuite renvoyer la commande reçu au Master.
     * En cas d'incompréhension nous envoyons : "000.00~"
     */
    void threadWorker();

    /**
     * @brief Equivalent du main pour le Thread associé à cette classe
     * utile pour faire fonctionner de manière 'classique' le threadWorker et de pouvoir y ajouter des sécurités
     */
     Callback<void()> main();

    /**
     * @brief Envoi de la mesure de l'élongation de chaque vérin
     *
     * @param leg_selected : (Leg_t) leg correspondant à la mesure
     * @param linear_actuator_selected : (LinearActuatorLeg_t) vérin sélectionné
     * @param mesure : (float) mesure de l'élongation en mm
     */
    void sendMesure(Leg_t leg_selected, LinearActuatorLeg_t linear_actuator_selected, float mesure);

    /**
     * @brief Dès qu'une commande est reç et interprétée nous la renvoyons au Master
     *
     * Renvoie de la commande interprétée au Master, si elle est erronée nous envoyons : "000.00~"
     *
     * @parm commandCheck : (bool) état de l'interprétation de la commande reçue
     */
    void sendCheck(bool commandCheck);

private:
    /**
     * @brief Interprète les données envoyée par le Master
     *
     * Réception des octets 1 à 1 et interprétation pour renseigner legSelect, linearActuatorSelect et cmdFloat. <br>
     * <br>
     * /!\ Ne pas oublier le caractère qui termine la transmission sinon aucun check n'est envoyé. <br>
     * Si le nombre de caractère constituant le caractère représentant la position demandée (float) dépasse MaxCharactersFloat (ConfigGlobal.h) alors nous renvoyons une erreur (-1).
     *
     * @param c : (char) octet réceptionné
     * @return 1  : si l'interprétation s'est bien passée
     * @return -1 : si l'interprétation s'est mal passée
     * @return 0  : si l'interprétation est toujours en cours
     */
    int8_t parseCmd(char c);

    /**
     * @brief Filtre pour la valeur demandée par Master
     *
     * Vérifie la faisabilité de la commande demandée
     *
     * @param targetDesired : (float) position en mm demandée par Master
     * @return true : si la commande est faisable
     * @return false : si la commande est infaisable
     */
    bool securityTarget(float targetDesired);

    Leg *legA;
    Leg *legB;
    EventFlags *emergencyFlag;
    Leg_t legSelected;
    LinearActuatorLeg_t linearActuatorSelected;
    BufferedSerial *serial;
    std::string cmdStr;
    float cmdFloat;
};


#endif //MEGABOT_DRIVER_MASTERCOMMUNICATION_H
