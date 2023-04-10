# Configuration de Mbed
Le projet est propulsé sur Mbed, nous l'utilisons en ligne de commande.

## Installation Mbed CLI 1 (Windows)
 - Mbed CLI 1 : celui utilisé pour ce tutoriel, l'original, simple et efficace, 
mais nécessite la présence d'un _.mbedignore_ à la racine du projet 
pour éviter de tout compiler (sinon c'est très très long sous Windows).

### 1] Le compilateur : ARM GCC
Quelle que soit la manière d'installer les outils Mbed sur votre machine, il vous faut d'abord installer la chaine de compilation nécessaire pour compiler un projet Mbed. La dernière version officielle ([maj du 1er mars 2023](https://os.mbed.com/docs/mbed-os/v6.16/build-tools/index.html)) est la 10.3-2021.07, recommandée depuis MbedOs 6.16.     
1. Télécharger et lancer l'installation de [GCC GNU ARM 10.3-2021.07](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.07/gcc-arm-none-eabi-10.3-2021.07win32/gcc-arm-none-eabi-10.3-2021.07-win32.exe).      
2. **ATTENTION** : à la fin de l'installation, bien cocher "*Add path to environment variable*".    
3. Optionnel : Pour vérifier l'installation, ouvrir une invite de commande (clique droit > "Cmder here") et taper `arm-none-eabi-gcc --version`    

### 3] Mbed en ligne de commande
Si vous êtes un peu plus à l'aise avec une invite de commande, vous pouvez installer Mbed CLI 1 à l'aide de python, et y avoir accès partout sur votre machine.    
**ATTENTION** : ne surtout pas utiliser l'installateur fourni par ARM ([ce truc](https://os.mbed.com/docs/mbed-os/v6.16/quick-start/build-with-mbed-cli.html)), c'est une ancienne version d'Mbed CLI qui utilise une ancienne version de python non recommandée.
1. Télécharger et installer [Mercurial/TortoiseHG](https://www.mercurial-scm.org/release/tortoisehg/windows/tortoisehg-6.3.2-x64.msi) (nécessaire pour Mbed CLI).
2. Aller dans Démarrer, puis taper "app execution aliases". Dans la fenêtre qui suit, désactiver toutes les options liées à Python.
> Long story short : Windows a décidé d'inclure Python dans le Windows Store, et force l'utilisateur à ouvrir le store dès qu'il tape "python" en ligne de commande. Au vu des avis sur Internet, et la manière donc est gérée l'interface de fichier par une app du Store, je ne recommande pas son utilisation.
3. Installation de Python 3.7.9, version actuellement supportée par MbedCLI 1:
   1. Si vous avez déjà cette version de Python d'installée, passez à la suite.
   2. Si vous avez Python 2.7 d'installée, vous devez quand même installer la 3.7.
   3. Si vous avez une version trop récente de python (genre 3.11), vous devez quand même installer la 3.7, ce n'est pas un problème, plusieurs versions de Python peuvent coexister sur un même OS, il faut juste ne pas se tromper sur celle qu'on appelle ...
   > Note : Il est possible d'installer Mbed CLI sous Python 3.11, mais au prix d'un long processus d'installation des outils Visual Studio Build Tool. Sauf si vraiment vous voulez le faire, je vous conseille plutôt d'installer la 3.7.   
   4. Télécharger et lancer l'installation [Python 3.7.9](https://www.python.org/ftp/python/3.7.9/python-3.7.9-amd64.exe).
   5. **ATTENTION** : Bien s'assurer de cocher "Add python.exe to PATH", puis faire "Install Now"
   6. À la fin de l'installation, faites le "Disable Path length limit".
4. Ouvrir une invite de commande et vérifier que les commandes suivantes fonctionnent :
   * `git --version`
   * `hg --version`
   * `python --version`
   * `arm-none-eabi-gcc --version`
5. Mettre à jour pip : `python.exe -m pip install --upgrade pip`
6. Installer pipx pour gérer des environnements isolés : `pip install --user pipx` (ne pas tenir compte des warnings sur le path)
7. Mettre à jour ses variables d'environnements Windows : `python.exe -m pipx ensurepath`
8. Redémarrer son terminal (fermer et rouvrir Cmder)
9. Installer Mbed-CLI 1.10.5 à l'aide de pipx : `pipx install mbed-cli`
10. Vérifier son installation : `mbed --version`

MbedCLI est désormais installé 🎉 !

### Les fichiers d'un projet MbedOS
![folder](https://global.discourse-cdn.com/standard17/uploads/mbed/original/2X/d/d12443b0ed678b2d3111c35dda9c44f0bda22f5e.png)
* `.mbedignore` : accélère la compilation en disant à MbedCLI 1 de ne pas tout compiler, très pratique sous Windows.
* `xxxx.lib` : Tout les fichiers se terminant par ".lib" sont des fichiers textes, avec l'URL de la librairie, suivi du numéro de commit associé à la version souhaitée.
* `.mbed` : le fichier de configuration du projet, avec au minimum la référence de la carte utilisée, et la toolchain utilisée (normalement GCC_ARM).

Pour mettre à jour le Cmake dans Clion: `mbed export -i cmake_gcc_arm`
Pour compiler : `mbed compile`
Pour téléverser : `mbed compile --flash`

Pour supprimer les fichiers compilés puis recompiler : `mbed compile --clean`

### Commençons à coder
Si vous avez déjà cloné le dépôt git ET qu'il est prêt pour mbed (avec les fichiers nécessaires) réaliser un `mbed deploy`

Sinon, pour importer le projet : `mbed import https://github.com/Eirlab/megabot-driver.git`

Au moment d'ajouter le dossier "mbed-os", MbedCLI installera tous les paquets Python manquant.

### *] Les commandes MbedCLI
Toutes ses commandes sont à faire à la racine du projet.
* Pour compiler le projet : `mbed compile`
* Pour uploader le programme sur une carte : `mbed compile --flash`
* Pour tout recompiler : `mbed compile --clean`
* Pour changer de carte : `mbed target NUCLEO_XXXX`
* Pour définir la toolchain GCC : `mbed toolchain GCC_ARM`
* Pour pouvoir ouvrir le dossier avec l'IDE [CLION](https://www.jetbrains.com/clion/) : `mbed export -i cmake_gcc_arm`
* Pour voir la liste des IDEs supportées : `mbed export -i soupe_au_choux`
* Pour importer un projet disponible sur internet : `mbed import <URL>`
* Pour déployer un projet après avoir fait un `git clone` au lieu d'un `mbed import`  : `mbed deploy`
* Pour ajouter une librairie trouvée sur internet : `mbed add <URL>`
* Pour mettre à jour une library, exemple mbed-os:
   * `cd mbed-os/`
   * `mbed sync`
   * `mbed update mbed-os-6.17.0` (le tag git, ou le numéro commit, définit la version)


# Megabot Driver
Le but de ce programme est de faire l'interface entre un PC central (ici appeler master) et les vérins de 2 pattes (6 vérins au total).

Pour cela, nous adoptons le diagramme de classe suivant : 
![Diagram Class](./Annexes/Diagram%20Class.png)

## Contexte
Nous allons créer 3 threads :
* _communication_master_ : gérant le protocol de communication pour l'exécution des mouvements
* _communication_LinearActuator_ : gérant la mise à jour des valeurs d'élongation
* _control_linearActuator_ : gérant l'exécution des ordres sur les vérins

Nous avons un flag _emergency_ qui va nous servir à savoir si nous avons perdu la communication avec les Arduino Nanos des vérins, si c'est le cas, il doit arrêter le contrôle des vérins.   
Nous aurons 6 objets _LinearActuator_ : 
* baseLegA
* middleLegA
* endLegA
* baseLegB
* middleLegB
* endLegB

Nous aurons 2 objets _Leg_ :
* legA
* legB

Nous aurons 1 instance de :
* MuxCommunication
* MasterCommunication 

### Séquences récurrentes
![Sequence Classique](./Annexes/Diagram%20Recurrent%20sequence.png)
La vérification sur les ports séries des Arduino et du master se fait très souvent.   
La mise à jour de la valeur de l'élongation des vérins se fait dès la réception des valeurs par les Nanos. La vérification d'_emergency_ se fait par la même occasion.
La vérification de l'_emergency_ sur le thread _control_linearActuator_ se fait dès qu'il y a une pwm d'active.


### Séquence classique
![Sequence Classique](./Annexes/Diagram%20Classic%20sequence%20Com.png)

Classiquement :
* le master nous envoi une commande
* nous répondons que nous avons bien reçu 
* nous exécutons la commande
* nous disons que nous avons terminé

### Séquence de danger
![Sequence Classique](./Annexes/Diagram%20Classic%20sequence%20Com.png)

S'il vient à y avoir un dysfonctionnement :
* nous arrêtons les vérins
* nous envoyons en boucle ?? 


## LinearActuator
Cette classe sert d'interface avec un vérin.  
Elle se construit avec :
* un identifiant, 
* les pins PWM, DIR1, DIR2 
* une référence vers _emergency_

Les fonctionnalités :
* commande en rampe pour les vérins.
* finalité simultanée sur l'exécution des commandes.   

Elle intègre la **sécurité** sur l'activation des vérins, si nous voyons que les Arduino Nanos n'ont pas envoyé de mesure depuis plus de 'timeOut' ms, nous considérons que nous sommes en **situation de danger**, nous arrêtons donc tous les vérins.

## Leg
Se construit avec :
* un identifiant,
* 2 référence vers _LinearActuator_

Encapsulation des différents _LinearActuator_ dans _legA_ et _legB_ pour faciliter leur gestion.

## MuxCommunication
Se construit avec :
* pins channelA, channelB, TXLegA, TXLegB
* 2 référence vers _Leg_

Gestion de la réception des positions des vérins et de la situation d'*emergency* (si nous ne recevons plus d'information).

## MasterCommunication
Cette classe sert d'interface avec le PC central.  
Elle se construit avec le port série 2 (le port série du port USB), des références vers les 2 Leg (A et B) d'une Nucleo.
Nous adoptons le protocole de communication suivant : 
* master -> nucleo
    * LegId | LinearActuatorId | target : pour demander à ce qu'un vérin aille à une certaine position    
* nucleo -> master 
    * LegId | LinearActuatorId | mesure (X : int | X : int | X : float): pour l'envoi des positions des vérins . Exemple : 4 |  2 | 160.2
    * ? : situation de danger, commande refusée
