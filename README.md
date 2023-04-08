# Configuration de Mbed
Le projet est propulsé sur Mbed, nous l'utilisons en ligne de commande.

## Installation Mbed CLI 1
 - Mbed CLI 1 : celui utilisé pour ce tutoriel, l'original, simple et efficace, 
mais nécessite la présence d'un _.mbedignore_ à la racine du projet 
pour éviter de tout compiler (sinon c'est très très long sous Windows).

### 1] Le compilateur : ARM GCC
<img src="https://chipcode-nl.gallerycdn.vsassets.io/extensions/chipcode-nl/gcc-arm-windows/1.0.1/1611260807399/Microsoft.VisualStudio.Services.Icons.Default" title="" alt="arm" data-align="center">   
Quelle que soit la manière d'installer les outils Mbed sur votre machine, il vous faut d'abord installer la chaine de compilation nécessaire pour compiler un projet Mbed. La dernière version officielle ([maj du 1er mars 2023](https://os.mbed.com/docs/mbed-os/v6.16/build-tools/index.html)) est la 10.3-2021.07, recommandée depuis MbedOs 6.16.     
1. Télécharger et lancer l'installation de [GCC GNU ARM 10.3-2021.07](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.07/gcc-arm-none-eabi-10.3-2021.07win32/gcc-arm-none-eabi-10.3-2021.07-win32.exe).      
2. **ATTENTION** : à la fin de l'installation, bien cocher "*Add path to environment variable*".    
3. Optionnel : Pour vérifier l'installation, ouvrir une invite de commande (clique droit > "Cmder here") et taper `arm-none-eabi-gcc --version`    

### 3] Mbed en ligne de commande
![mbedcli](https://mblogthumb-phinf.pstatic.net/MjAxOTA2MjdfMTE1/MDAxNTYxNjA4MzQyNDIy.0-AWnmuA7St2lKvY5A5a3OagrrRxpaU3QkI9TnhHJRQg.9aDioLnM_6aa45FzOo-cUBk1YG3_JZM9r1ax5l6FY6Yg.PNG.roboholic84/K-20190627-470232.png?type=w2)
Si vous êtes un peu plus à l'aise avec une invite de commande, vous pouvez installer Mbed CLI 1 à l'aide de python, et y avoir accès partout sur votre machine.    
**ATTENTION** : ne surtout pas utiliser l'installateur fourni par ARM ([ce truc](https://os.mbed.com/docs/mbed-os/v6.16/quick-start/build-with-mbed-cli.html)), c'est une ancienne version d'Mbed CLI qui utilise une ancienne version de python non recommandée.
1. Si vous utilisez "Cmder", vous n'avez pas besoin d'installer Git for Windows (sinon, [installez-le](https://gitforwindows.org/)).
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
12. Pour importer le projet dans son IDE : `mbed import https://github.com/Eirlab/megabot-driver.git`
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

Pour mettre à jour le Cmake dans Clion: `mbed export -i cmake_gcc_arm`
Pour compiler : `mbed compile`
Pour téléverser : `mbed compile --flash`

Pour supprimer les fichiers compilés puis recompiler : `mbed compile --clean`

# Megabot Driver
Le but de ce programme est de 