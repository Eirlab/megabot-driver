# Guide "Saint Graal" d'installation Mbed sous Windows - Projet template Eirbot

<img title="" src="https://avatars.githubusercontent.com/u/8916057?s=280&v=4" alt="mbed" width="159"> <img title="" src="https://www.breizh-info.com/wp-content/uploads/2022/12/fusion.jpeg" alt="a" width="287"> <img title="" src="https://eirbot.github.io/images/LogoEirbot.png" alt="eirbot" width="176">



Ce répo est un projet de base pour pouvoir utiliser Mbed sur les Nucleo F446RE de l'association Eirbot. Il est accompagné de ce README pour vous expliquer comment installer et utiliser MbedOS sur Windows.

MbedOs est un *RTOS* (Real Time Operating System). Pour faire (très) court, comparé à Arduino, vous pouvez faire plusieurs tâche en parallèle (plusieurs *Threads*). Par exemple une tâche pour l'odométrie, une tâche pour l'asservissement moteur, une tâche pour la stratégie du robot ... Plus besoin de s'arracher les cheveux dans sa fonction `loop()` d'Arduino pour tout faire fonctionner en même temps.

ARM fournit deux outils en ligne de commande pour compiler un projet Mbed :

* [Mbed CLI 1](https://github.com/ARMmbed/mbed-cli) : celui utilisé pour ce tutoriel, l'original, simple et efficace, mais nécessite la présence d'un `.mbedignore` à la racine du projet pour éviter de tout compiler (sinon c'est très très long sous Windows).

* [Mbed CLI 2](https://github.com/ARMmbed/mbed-tools), ou "mbed-tools" : Une nouvelle version d'Mbed CLI 1 dont le fonctionnement est basé sur CMake, mais l'outil est toujours en bêta, et n'a pas été mis à jour depuis plus d'un an, donc à éviter pour le moment.

De plus, ARM fournit un éditeur tout-en-un qui permet de déployer, compiler et uploader sur une carte compatible Mbed : **MbedStudio**. Pour les jeunes loutres qui ont peur de l'invite des commandes, c'est l'option préférée.

Plusieurs types d'installations seront vus dans ce README, à vous de voir celle qui vous ![]()correspond le mieux. 

*NOTE : ce tutoriel est tourné vers les personnes qui utilisent principalement **Windows**, soit une très grande partie d'Eirbot. Si vous voulez installer MbedCLI sur Linux/Ubuntu, je vous conseille de jeter un oeil sur les forums et autres méthodes d'installation proposé par Arm Mbed.*

> ***J'ai déjà PlatformIO, pourquoi ne pas l'utiliser au lieu de s'embêter à installer d'autres outils ?***
> 
> Parce que les packages de PlatformIO n'ont pas été mis à jour depuis trop longtemps. La dernière version officielle d'MbedOs [est actuellement la 6.17](https://github.com/ARMmbed/mbed-os/releases), alors que [PIO est resté bloqué sur la 6.9](https://registry.platformio.org/tools/platformio/framework-mbed/versions). Beaucoup trop de changement entre les deux versions rend l'intercompatibilité quasi impossible. Pour le moment, je recommande d'utiliser les outils fournis par Mbed pour profiter des dernières version du *RTOS*.

**Avant de commencer** : Il est conseillé de lire tout le tutoriel, notamment les lignes commençant par "**ATTENTION**", au risque de griller des étapes et de devoir, par exemple, modifier ses variables d'environnement pour rectifier le tir ...

## 0] Un invite de commande améliorée : Cmder

<img src="https://camo.githubusercontent.com/73651fee2f0ec76a2dc55be36373ed61a63dc80b90cfe2763cf294a24444b587/687474703a2f2f692e696d6775722e636f6d2f67316e4e6630492e706e67" title="" alt="a" data-align="center">

Cette étape est facultative, mais je la conseille fortement pour les utilisateurs Windows. Même si vous êtes allergiques aux invites de commandes, ça permet à un ancien de vous aider rapidement. 

[Cmder](https://cmder.app/) est une invite de commande améliorée, avec quelques raccourcis pratiques comme sous Linux. C'est un fork de [Conemu](https://conemu.github.io/), avec [Clink](https://mridgers.github.io/clink/) déjà installé, et qui intègre aussi [Git for Windows](https://gitforwindows.org/).

1. Si ce n'est pas déjà fait, [télécharger et installer 7zip](https://www.7-zip.org/a/7z2201-x64.exe).

2. Télécharger la dernière version de Cmder, en "full package" ([actuellement la 1.3.21](https://github.com/cmderdev/cmder/releases/download/v1.3.21/cmder.zip))

3. Copié collé l'archive dans vos documents.

4. Dézipper l'archive avec 7zip (clique droit > "7-Zip" > "Extraire vers cmder\" ).

5. Ouvrir le dossier "cmder", cliquer sur "Cmder.exe".

6. Taper la commande suivante : `cmder.exe /REGISTER ALL`

7. Vous pouvez désormais faire clic droit > "Cmder here" dans n'importe quel dossier sur Windows.

8. Optionnel : rajouter l'icône de Cmder dans votre barre des tâches.

## 1] Le compilateur : ARM GCC

<img src="https://chipcode-nl.gallerycdn.vsassets.io/extensions/chipcode-nl/gcc-arm-windows/1.0.1/1611260807399/Microsoft.VisualStudio.Services.Icons.Default" title="" alt="arm" data-align="center">

Quelle que soit la manière d'installer les outils Mbed sur votre machine, il vous faut d'abord installer la chaine de compilation nécessaire pour compiler un projet Mbed. La dernière version officielle ([maj du 1er mars 2023](https://os.mbed.com/docs/mbed-os/v6.16/build-tools/index.html)) est la 10.3-2021.07, recommandée depuis MbedOs 6.16.

1. Télécharger et lancer l'installation de [GCC GNU ARM 10.3-2021.07](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.07/gcc-arm-none-eabi-10.3-2021.07win32/gcc-arm-none-eabi-10.3-2021.07-win32.exe).

2. **ATTENTION** : à la fin de l'installation, bien cocher "*Add path to environment variable*".

3. Optionnel : Pour vérifier l'installation, ouvrir une invite de commande (clique droit > "Cmder here") et taper `arm-none-eabi-gcc --version`

## 2] Choix 1 : MbedStudio

![mbed studio](https://os.mbed.com/docs/mbed-studio/current/editing-linting/images/navigate_code.gif)

L'IDE MbedStudio permet de coder rapidement avec MbedOs. C'est une version modifiée de VsCode, avec une interface simplifiée vous permettant de compiler, uploader, ouvrir un port COM, sans toucher à une invite de commande. Attention en revanche, il sera nécessaire de créer un compte Arm Mbed pour accéder ensuite à l'IDE (gratuit).

La manipulation qui suit n'est à faire qu'une seule fois, vous pourrez ensuite ouvrir ou importer n'importe quel projet Mbed en ouvrant MbedStudio.

1. Installez MbedStudio 1.4.1 : [lien de téléchargement](https://studio.mbed.com/installers/latest/win/MbedStudio-1.4.1.exe)
   **Ne surtout pas installer la dernière version (1.4.4)**, l'intelissense de l'IDE est bugué avec le compilateur GCC.
   *Si vous avez une version plus récente, désinstallez là avant d'installer la 1.4.1.*

2. Ouvrir MbedStudio, et créer son compte si ce n'est déjà fait.

3. Dans "File" > "Import Program...", copiez le lien du dépôt github eirbot :
   `https://github.com/eirbot/mbed-os-template-eirbot`
   
   Puis cliquez sur "Add program".

4. **Fermez** MbedStudio.

5. Ouvrez le dossier du projet Eirbot. Par défaut, il devrait être dans :
   `C:\Users\<VOTRE_NOM>\Mbed Programs\mbed-os-template-eirbot\`

6. Copiez le fichier `external-tools.json`.

7. Aller dans le dossier de configuration d'MbedStudio, se situant dans :
   `C:\Users\<VOTRE_NOM>\AppData\Local\Mbed Studio`

8. Collez le fichier `external-tools.json` directement à la racine du dossier.

Vous pouvez désormais ouvrir MbedStudio, et compiler le programme d'exemple d'eirbot pour la nucleo F446RE. l'Intellisense dans le code devrait fonctionner.

## 3] Choix 2 : Mbed en ligne de commande

![mbedcli](https://mblogthumb-phinf.pstatic.net/MjAxOTA2MjdfMTE1/MDAxNTYxNjA4MzQyNDIy.0-AWnmuA7St2lKvY5A5a3OagrrRxpaU3QkI9TnhHJRQg.9aDioLnM_6aa45FzOo-cUBk1YG3_JZM9r1ax5l6FY6Yg.PNG.roboholic84/K-20190627-470232.png?type=w2)

Si vous êtes un peu plus à l'aise avec une invite de commande, vous pouvez installer Mbed CLI 1 à l'aide de python, et y avoir accès partout sur votre machine.

**ATTENTION** : ne surtout pas utiliser l'installateur fourni par ARM ([ce truc](https://os.mbed.com/docs/mbed-os/v6.16/quick-start/build-with-mbed-cli.html)), c'est une ancienne version d'Mbed CLI qui utilise une ancienne version de python non recommandée.

0. Si vous utilisez "Cmder", vous n'avez pas besoin d'installer Git for Windows (sinon, [installez-le](https://gitforwindows.org/)).

1. Télécharger et installer [Mercurial/TortoiseHG](https://www.mercurial-scm.org/release/tortoisehg/windows/tortoisehg-6.3.2-x64.msi) (nécessaire pour Mbed CLI).

2. Aller dans Démarrer, puis taper "app execution aliases". Dans la fenêtre qui suit, désactiver toutes les options liées à Python.

> Long story short : Windows a décidé d'inclure Python dans le Windows Store, et force l'utilisateur à ouvrir le store dès qu'il tape "python" en ligne de commande. Au vu des avis sur Internet, et la manière donc est gérée l'interface de fichier par une app du Store, je ne recommande pas son utilisation.

<details>
  <summary>Sources</summary>

* https://github.com/mbed-ce/mbed-os/wiki/Toolchain-Setup-Guide

* https://dev.to/naruaika/why-i-didn-t-install-python-from-the-microsoft-store-5cbd

* https://docs.python.org/3/using/windows.html#known-issues

* https://stackoverflow.com/questions/56974927/permission-denied-trying-to-run-python-on-windows-10/57447610#57447610

* https://devrant.com/rants/2283963/who-the-fuck-at-microsoft-decided-to-put-python-on-their-store-i-don-t-care-if-i

* https://stackoverflow.com/a/57421906   

* https://www.reddit.com/r/learnpython/comments/tsqsrt/python_38_from_windows_store_not_working_no/

* https://github.com/python-poetry/poetry/issues/5331#issuecomment-1169731615

</details>

3. Installation de Python 3.7.9, version actuellement supportée par MbedCLI 1:
   
   1. Si vous avez déjà cette version de Python d'installée, passez à la suite.
   
   2. Si vous avez Python 2.7 d'installée, vous devez quand même installer la 3.7.
   
   3. Si vous avez une version trop récente de python (genre 3.11), vous devez quand même installer la 3.7, ce n'est pas un problème, plusieurs versions de Python peuvent coexister sur un même OS, il faut juste ne pas se tromper sur celle qu'on appelle ...
   
   > Note : Il est possible d'installer Mbed CLI sous Python 3.11, mais au prix d'un long processus d'installation des outils Visual Studio Build Tool. Sauf si vraiment vous voulez le faire, je vous conseille plutôt d'installer la 3.7.
   
   4. Télécharger et lancer l'installation [Python 3.7.9](https://www.python.org/ftp/python/3.7.9/python-3.7.9-amd64.exe).
   5. **ATTENTION** : Bien s'assurer de cocher "Add python.exe to PATH", puis faire "Install Now"
   6. À la fin de l'installation, faites le "Disable Path length limit".

4. Ouvrir une invite de commande (clique droit + "Cmder Here") et vérifier que les commandes suivantes fonctionnent :
   
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

11. Aller dans vos documents, puis ouvrir un terminal.

12. Importer le template eirbot : `mbed import https://github.com/eirbot/mbed-os-template-eirbot`

13. Au moment d'ajouter le dossier "mbed-os", MbedCLI installera tous les paquets Python manquant.

MbedCLI est désormais installé 🎉 !

## 4] Les commandes MbedCLI

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

* Pour ajouter une librairie trouvé sur internet : `mbed add <URL>`

* Pour mettre à jour une library, exemple mbed-os:
  
  * `cd mbed-os/`
  
  * `mbed sync`
  
  * `mbed update mbed-os-6.17.0` (le tag git, ou le numero commit, définit la version)

## 5] Les fichiers d'un projet MbedOS

![folder](https://global.discourse-cdn.com/standard17/uploads/mbed/original/2X/d/d12443b0ed678b2d3111c35dda9c44f0bda22f5e.png)

* `.mbedignore` : accélère la compilation en disant à MbedCLI 1 de ne pas tout compiler, très pratique sous Windows.

* `xxxx.lib` : Tout les fichiers se terminant par ".lib" sont des fichiers textes, avec l'URL de la librairie, suivi du numéro de commit associé à la version souhaitée. 

* `.mbed` : le fichier de configuration du projet, avec au minimum la référence de la carte utilisée, et la toolchain utilisée (normalement GCC_ARM).

## TODO

- Expliquer différences entre ARM GCC et ARMC6.

- Expliquer comment installer MbedCLI 1 sous Python 3.11 avec les outils Build Tool de Visual Studio.

- Ajouter des images/screens/gifs ? Utiliser des images perso au lieu d'internet.

- Terminer le point numéro 5 (WIP).
