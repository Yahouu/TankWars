/*  ***REMOVED***
    ***REMOVED***

    Projet de programmation en C - 1ère année
    Tank Wars
    Construit avec SDL 2.0
    */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"

// On définit notre tableau de jeu

#define BLOC                46                  // Dimensions en pixels d'un bloc
#define DIMTANK             34                  // Dimensions d'un tank
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           506                 // Dimensions de notre fenêtre

// On définit nos éléments de jeu. Leur valeur ne nous importe pas.

enum liste_terrain {NORMAL, MINE, POLLUE, BASE1, BASE2, TANK, COMMAND};

void pause();

int main(int argc, char *argv[])
{
    // On initialise SDL et on vérifie immédiatement si la librairie s'est bien lancée

    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    // Notre terrain de jeu

    int terrain[11][11] = {
   { 3,3,3,3,3,1,0,0,0,0,0 },
   { 3,1,3,3,0,0,0,0,0,1,0 },
   { 3,3,3,1,0,1,0,1,0,0,0 },
   { 3,3,1,0,0,0,0,0,1,0,0 },
   { 3,0,0,0,2,2,2,0,0,0,0 },
   { 1,0,1,0,2,2,2,0,1,0,1 },
   { 0,0,0,0,2,2,2,0,0,0,4 },
   { 0,0,1,0,0,0,0,0,1,4,4 },
   { 0,0,0,1,0,1,0,1,4,4,4 },
   { 0,1,0,0,0,0,0,4,4,1,4 },
   { 0,0,0,0,0,1,4,4,4,4,4 },
    };

    // Création de la fenêtre

    SDL_Window* ecran = NULL;
    ecran = SDL_CreateWindow("TankWars",SDL_WINDOWPOS_UNDEFINED,    // Position de la fenêtre sur l'écran de l'utilisateur
                                        SDL_WINDOWPOS_UNDEFINED,    // Ici ça ne nous importe pas
                                        DIMENSION,                  // Dimensions
                                        DIMENSION,
                                        SDL_WINDOW_SHOWN);          // On affiche la fenêtre, cachée par défaut

    // Création des sprites

    SDL_Surface *sp_terrain = SDL_LoadBMP("./carte.bmp"), *sp_placeholder = IMG_Load("./mur.jpg");

    // On vérifie si la fenêtre puis les différents sprites ont bien été chargés ; ils prennent alors la valeur TRUE et on procède

    if( ecran )
    {
        if ( sp_terrain && sp_placeholder )
        {
            SDL_Rect dest_sp_terrain = { 0, 0, 0, 0};  // On définit un rectangle dans la fenêtre pour placer notre sprite
            SDL_Rect dest_sp_placeholder = {BLOC/2 - TANK/2,BLOC/2 - TANK/2,0,0};
            SDL_BlitSurface(sp_terrain,NULL,SDL_GetWindowSurface(ecran),&dest_sp_terrain); // Copie du sprite
            SDL_BlitSurface(sp_placeholder,NULL,SDL_GetWindowSurface(ecran),&dest_sp_placeholder);

            SDL_UpdateWindowSurface(ecran); // Mise à jour de la fenêtre pour prendre en compte la copie du sprite

        }

        else
        {
                fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
        }

    }
    else
    {
            fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
    }

    pause();
    SDL_FreeSurface(sp_terrain);
    SDL_DestroyWindow(ecran);
    SDL_Quit();

    return 0;
}

void pause()
{
    /*  Cette procédure permet de pauser le programme quand il a finit d'executer toutes les instructions.
        Cela laisse le temps à l'utilisateur d'intéragir avec le jeu et de quitter le jeu par lui-même
        */

    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:  // On quitte le jeu quand on appuie sur la croix rouge habituelle
                continuer = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: // On quitte quand on appuie sur la touche Echap/Escape
                        continuer = 0;
                        break;
                }
                break;
        }
    }
}
