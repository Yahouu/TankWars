#TankWar

##Objectifs

La Bataille de chars est un jeu moderne polonais qui est joué sur un plateau carré de 11x11 cases. Chaque joueur gère son armée de 14 chars - 13 normaux et un, le char du commandant, qui est suffisamment résistant pour se déplacer à travers le terrain contaminé par des armes chimiques. Au départ, les chars des commandants sont situés dans le coin en bas à droite (du point de vue de chaque joueur) et les chars normaux occupent la zone restante de la base (cases bleues pour le joueur blanc et case verte pour le joueur noir).

Le but est d'occuper la base ennemie par autant de ses chars que possible. Le joueur qui obtient le plus de points est le gagnant. Le nombre de points correspond aux événements du jeu et seront décrits à la section finale de cet article.

![alt text](http://i.imgur.com/Vr6V95N.jpg "Plateau de jeu")

##Buts du jeu

####Déplacement des chars

Chaque joueur peut bouger un char par tour. Les chars peuvent se déplacer de plusieurs cases verticalement, horizontalement ou en diagonale avec ces conditions:
  - Un char ne peut pas se déplacer sur les cases noires (mines).
  - Un char ne peut pas sauter par dessus un char allié ou adverse.
  - Seul le char du commandant peut aller sur les cases rouges (ou les traverser) au centre du plateau (zone polluée).
  - Les chars des commandants ne peuvent se capturer l'un l'autre dans la zone rouge polluée du centre.
  - Le char qui a atteint la base adverse (cases vertes ou bleues) ne peut pas quitter cette base jusqu'à la fin de la partie.

####Comment capturer les chars ennemis :

La partie est terminée si un joueur amène tous ses chars non détruits dans la base ennemie. A ce moment, le score final est calculé et le joueur qui a le plus de points gagne. Dans le cas d'un même nombre de points pour chaque joueur la partie est nulle.

Les points sont comptés de la manière suivante:

 - 3 points pour le char du commandant qui se trouve sur la case de départ du char commandant adverse (la case marquée d'un cercle).
 - 2 points pour un char de commandant qui se trouve sur n'importe quelle case de la base ennemie.
 - 2 points pour un char ordinaire qui se trouve sur n'importe quelle case de la base ennemie.
 - 1 point pour n'importe quel char (ordinaire ou commandant) qui reste sur le champ de bataille et qui n'est pas détruit.

![alt text](http://i.imgur.com/WvEJlta.jpg?1 "Décompte des points")

Le joueur blanc a 2 points pour le char du commandant dans la base ennemie et 2 x 1 point pour deux chars sur le champ de bataille, le résultat donne 4 points. Le joueur noir obtient 3 points pour le char du commandant sur la case de départ du char commandant blanc et 2 x 2 points pour deux chars dans la base ennemie, le résultat donne 7 points. Noir et le vainqueur.

##Version

0.6

##Installation

 - Installer SDL2 et la librairie SLD_Image pour votre OS et IDE favoris
 - Ajouter les flags de compilation nécessaires. Exemple pour Windows et Codeblocks:
 ```
 -IC:\$PATH\include\SDL2 -IC:\$PATH\include -LC:\$PATH\lib -lSDL2_image -lmingw32 -lSDL2main -lSDL2.dll -luser32 -lgdi32 -lwinmm -ldxguid
```
 - Copier les dll pour SDL2 et SDL_Image dans le dossier du projet


##Technologies 

* [C] - langage de programmation qu'on ne présente plus
* [SDL2] - librairie de développement de jeux vidéo en 2D ou 3D

[C]:https://en.wikipedia.org/wiki/C_%28programming_language%29
[SDL2]:http://www.libsdl.org/
