# Projet template Mbed pour Eirbot

Ce répo est un projet de base pour pouvoir utiliser Mbed sur les Nucleo F446RE de l'association Eirbot.

L'IDE MbedStudio permet de coder rapidement sur ce framework.

## Installation d'MbedStudio et configuration de GCC

Ce tutoriel est fait pour les utilisateurs de Windows. La manipulation n'est à faire qu'une seule fois, vous pourrez ensuite importer n'importe quel projet Mbed correctement.

1. Installez MbedStudio 1.4.1 : [lien de téléchargement](https://studio.mbed.com/installers/latest/win/MbedStudio-1.4.1.exe)
   Ne surtout pas installer la dernière version (1.4.4), l'intelissense de l'IDE est bugué avec le compilateur GCC.
   Si vous avez une version plus récente, désinstallez là avant d'installer la 1.4.1.

2. Installez GCC ARM 9 2019 Q4 Major : [lien de téléchargement](https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-win32-sha2.exe)
   C'est la version de GCC actuellement recommandé par Mbed.

3. Ouvrir MbedStudio, et créer son compte si ce n'est déjà fait.

4. Dans "File" > "Import Program...", copiez le lien du dépot github eirbot :
   `https://github.com/eirbot/mbed-os-template-eirbot`
   
   Puis cliquez sur "Add program".

5. **Fermez** MbedStudio.

6. Ouvrez le dossier du projet Eirbot. Par défaut, il devrait être dans :
   `C:\Users\<UTILISATEUR>\Mbed Programs\mbed-os-template-eirbot\`

7. Copiez le fichier `external-tools.json`.

8. Allez dans le dossier de configuration d'MbedStudio, se situant dans :
   `C:\Users\<UTILISATEUR>\AppData\Local\Mbed Studio`

9. Collez le fichier `external-tools.json` directement à la racine du dossier.



Vous pouvez désormais ouvrir MbedStudio, et compiler le programme d'exemple d'eirbot pour la nucleo F446RE.


