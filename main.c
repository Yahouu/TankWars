/*  Projet de programmation en C - 1ère année
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
#define DIMTANK             32                  // Dimensions d'un tank
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           506                 // Dimensions de notre fenêtre

// On définit nos éléments de jeu. Leur valeur ne nous importe pas.

enum liste_terrain {NORMAL, MINE, POLLUE, BASE1, BASE2, TANK, COMMAND};

void pause();
//void close();
bool initialisation();

int main(int argc, char *argv[])
{
    if (!initialisation())
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    // Notre terrain de jeu

    int terrain[LARGEUR][HAUTEUR] = {
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

    // Initialisation de nos pointeurs

    SDL_Window* ecran = NULL;
    SDL_Surface *sp_terrain_tmp = NULL, *sp_terrain = NULL, *sp_placeholder = NULL;

    // Création de la fenêtre

    ecran = SDL_CreateWindow("TankWars",SDL_WINDOWPOS_UNDEFINED,    // Position de la fenêtre sur l'écran de l'utilisateur
                                        SDL_WINDOWPOS_UNDEFINED,    // Ici ça ne nous importe pas
                                        DIMENSION,                  // Dimensions
                                        DIMENSION,
                                        SDL_WINDOW_SHOWN);          // On affiche la fenêtre, cachée par défaut

    // Création des sprites

    sp_terrain_tmp = SDL_LoadBMP("./carte.bmp");
    sp_placeholder = SDL_LoadBMP("mario.bmp");

    // On vérifie si la fenêtre puis les différents sprites ont bien été chargés ; ils prennent alors la valeur TRUE et on procède

    if( ecran )
    {
        if ( sp_terrain_tmp && sp_placeholder )
        {
            SDL_Rect dest_sp_placeholder = { BLOC/2 - DIMTANK/2, BLOC/2 - DIMTANK/2, 0, 0};  // On définit un rectangle dans la fenêtre pour placer notre sprite
            sp_terrain = SDL_ConvertSurface( sp_terrain_tmp, SDL_GetWindowSurface(ecran)->format, NULL );
            SDL_FreeSurface( sp_terrain_tmp );
            SDL_BlitSurface(sp_terrain,NULL,SDL_GetWindowSurface(ecran),NULL); // Copie du sprite &dest_sp_terrain
            SDL_BlitSurface(sp_placeholder,NULL,SDL_GetWindowSurface(ecran),&dest_sp_placeholder);
            SDL_UpdateWindowSurface(ecran);
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
    IMG_Quit();
    SDL_FreeSurface(sp_terrain);
    SDL_FreeSurface(sp_placeholder);
    sp_terrain = NULL;
    sp_placeholder = NULL;
    SDL_DestroyWindow(ecran);
    ecran = NULL;
    SDL_Quit();

    return 0;
}

bool initialisation()
{
    bool init=true;

    // On initialise SDL et on vérifie immédiatement si la librairie s'est bien lancée

    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        init=false;
    }

    // SDL_Image

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stdout,"Echec de l'initialisation du support des png et jpg par IMG_Init (%s)\n",IMG_GetError());
        init=false;
    }

    return init;

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

/*void close()
{
    Cette procédure vide la mémoire, pointe nos pointeurs sur NULL et quitte SDL

    IMG_Quit();
    SDL_FreeSurface(sp_terrain);
    SDL_FreeSurface(sp_placeholder);
    sp_terrain = NULL;
    sp_placeholder = NULL;
    SDL_DestroyWindow(ecran);
    ecran = NULL;
    SDL_Quit();

}*/
