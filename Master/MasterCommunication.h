#ifndef MEGABOT_DRIVER_MASTERCOMMUNICATION_H
#define MEGABOT_DRIVER_MASTERCOMMUNICATION_H

#include "mbed.h"
#include "Leg.h"
#include "LinearActuator.h"
#include "GlobalConfig.h"

class MasterCommunication {
    //TODO Documentation
public:
    /**
     * @brief Création d'un thread autonome permettant de recevoir les commandes pour les vérins et envoyer les mesures de leur élongation.
     *
     * @paragraph Nous créons donc un thread qui à le comportement suivant : <br>
     * Lorsque nous réceptionnons une commande nous l'interprétons (parseCmd) et si elle est bien comprise nous la renvoyons sinon nous renvoyons "000.00~". <br>
     * Lorsque nous réceptionnons une mesure d'élongation nous la transmettons au master.<br>
     * Lorsque nous rencontrons un problème lors de la mesure de l'élongation nous levons un flag et nous envoyons, dans la mesure du possible, de quel vérin vient le problème. <br>
     * Par exemple, si le vérin 2 de la patte 1 dysfonctionne, nous renvoyons : "12-1.00~"<br>
     * @param master : (BufferedSerial*) pointeur vers le port série avec le Master
     * @param main_flag : (EventFlags*) pointeur vers le gestionnaire de flag
     * @param leg_A : (Leg*) pointeur vers la patte A
     * @param leg_B : (Leg*) pointeur vers la patte B
     */
    MasterCommunication(BufferedSerial *master, EventFlags *main_flag, Leg *leg_A, Leg *leg_B);
private:
    /**
     * @brief Fonctionnement de la communication dans le Thread. <br>
     *
     * Cette fonction s'apparente à la fonction `main()` dans un *main.cpp*.<br>
     * Elle va attendre qu'un caractère soit reçu du Master, <br>
     * puis appeler la fonction `parseCmd` qui va interpréter les caractère 1 à 1, et renvoyer un indicateur sur la validité de la commande reçue,
     * S'il y a une une compréhension parfaite, nous renvoyons la commande au Master, sinon nous envoyons : "000.00~" <br>
     * <br>
     * Il y a aussi une gestion de l'envoie des mesures des élongations de chacun des vérins, dès qu'on a reçu l'information, nous l'envoyons à la classe LinearActuator (via la classe Leg),
     * nous levons donc à ce moment là un flag (configurable dans le GlobalConfig) pour tenter d'identifier le vérin qui dysfonctionne.
     * En cas de dysfonctionnement nous nous mettons en sécurité, et nous renvoyons en boucle : "XX-1.00~" (ex: pour le vérins 2 de la patte 1 : "12-1.00~")
     *
     */
    void main();

    /**
     * @brief Envoi de la mesure de l'élongation de chaque vérin
     *
     * @param leg_selected : (Leg_t) leg correspondant à la mesure
     * @param linear_actuator_selected : (LinearActuatorLeg_t) vérin sélectionné
     * @param mesure : (float) mesure de l'élongation en mm
     */
    void sendMesure(LegId_t leg_selected, LinearActuatorId_t linear_actuator_selected, float mesure);

    /**
     * @brief Dès qu'une commande est reç et interprétée nous la renvoyons au Master
     *
     * Renvoie de la commande interprétée au Master, si elle est erronée nous envoyons : "000.00~"
     *
     * @parm commandCheck : (bool) état de l'interprétation de la commande reçue
     */
    void sendCheck(bool commandCheck);

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

    Thread communication_master; //< Thread de fonctionnement de
    EventFlags *flag;

    BufferedSerial *serial;
    std::string cmdStr;
    float cmdFloat;

    Leg *legA;
    Leg *legB;
    LegId_t legSelected;
    LinearActuatorId_t linearActuatorSelected;

};


#endif //MEGABOT_DRIVER_MASTERCOMMUNICATION_H
