/** Projet de programmation en C - 1ère année
 *  TankWar
 *  Construit avec SDL 2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

// On définit notre tableau de jeu

#define BLOC                65                  // Dimensions en pixels d'un bloc
#define DIMTANK             64                  // Dimensions d'un tank
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           716                 // Dimensions de notre carte
#define DIMEXTRA            598                 // Largeur de la fenêtre

// On définit nos éléments de jeu. Leur valeur ne nous importe pas.

enum liste_terrain {NORMAL, MINE, POLLUE, BASE1, BASE2};
enum liste_tank {VIDE, TANK1, TANK2, TANK1_CMD, TANK2_CMD};
enum liste_deplacement {INVALIDE, VALIDE, ENNEMI};

// Cette structure contient toutes les informations affectant une case (tile) du terrain

typedef struct struct_tile{
	int terrain;
	int tank;
	int autorise;	// le tank peut se déplacer sur cette case
}struct_tile;

// Nos textures

typedef struct struct_textures{
    SDL_Texture *tex_terrain = NULL;
    SDL_Texture *tex_tank[5] = {NULL};
    SDL_Texture *tex_tank_mvt[5] = {NULL};
    SDL_Texture *tex_highlight = NULL;
}struct_textures;

// Cette structure contient les informations sur le tank à déplacer

typedef struct struct_tank{
	int x,y;
	int type;
}struct_tank;

// Prototypes

int genererCarte(struct_tile tab_S_tile[][LARGEUR]);
int initialisation(struct_tile tab_S_tile[][LARGEUR]);
void loadTextures( struct_textures *S_textures );
int menu();
void generateTextures( struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] );
void deplacerTank(int *joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] );
int collision(int joueur, int i, int j, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank);
void deplacementsPossibles( int joueur, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank );
void pause();
void close( struct_textures *S_textures );

// Fenêtre de jeu et renderer

SDL_Window *ecran = NULL;
SDL_Renderer *renderer = NULL;

/************************************************************************************************
 *                                          FONCTIONS                                           *
 ************************************************************************************************/

 int main(int argc, char *argv[])
{
	struct_tile tab_S_tile[HAUTEUR][LARGEUR];
	struct_textures S_textures;

	// On vérifie que tout s'est bien initialisé

    if (!initialisation(tab_S_tile))
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    // On choisit le joueur qui commence au hasard

    srand(time(NULL));
    int joueur = ((rand()%2)+1);

    loadTextures( &S_textures );

    if (menu() == -1)
    {
        close( &S_textures );
    }

    else
    {
        // On génère notre terrain pour la première fois

        SDL_RenderClear( renderer );
        generateTextures( &S_textures, tab_S_tile );
        SDL_RenderPresent( renderer );

        // Boucle infinie pour bouger un tank
        deplacerTank( &joueur, &S_textures, tab_S_tile );

        pause();
        close( &S_textures );
    }

    return 0;
}

int initialisation(struct_tile tab_S_tile[][LARGEUR])
{
    /** Cette fonction permet d'initialiser toutes les autres fonctions nécessaire au lancement du jeu.
     *  On initialise SDL, SDL_Image, le renderer et on crée la fenêtre de jeu.
     *  Si on a un problème, un message d'erreur est affiché dans la console.
     *
     *  Retourne 1 si tout a été initialisé, 0 sinon
     */

    int init=1;

    // On initialise SDL et on vérifie immédiatement si la librairie s'est bien lancée

    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        init=0;
    }

    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        fprintf(stdout, "Échec de l'initialisation du filtrage linéaire de textures!" );
    }

    // Création de la fenêtre

    ecran = SDL_CreateWindow("TankWars",SDL_WINDOWPOS_UNDEFINED,    // Position de la fenêtre sur l'écran de l'utilisateur
                                        SDL_WINDOWPOS_UNDEFINED,    // Ici ça ne nous importe pas
                                        DIMENSION,                  // Dimensions
                                        DIMENSION,
                                        SDL_WINDOW_SHOWN);          // On affiche la fenêtre, cachée par défaut

    if( ecran == NULL )
    {
        printf( "Échec de la création de la fenêtre: %s\n", SDL_GetError() );
        init = 0;
    }

    // Création du renderer

    renderer = SDL_CreateRenderer( ecran, -1, SDL_RENDERER_ACCELERATED );

    if( ecran == NULL )
    {
        printf( "Échec de la création du renderer: SDL Error: %s\n", SDL_GetError() );
        init = 0;
    }

    // La couleur par défaut dans la fenêtre est le blanc

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // Initialisation de SDL_Image

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stdout,"Echec de l'initialisation du support des png et jpg par IMG_Init (%s)\n",IMG_GetError());
        init=0;
    }

    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Échec de l'initialisation de SDL_TTF : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    init = genererCarte(tab_S_tile);

    return init;

}

int genererCarte(struct_tile tab_S_tile[][LARGEUR])
{
    FILE* carte = NULL;
    char ligneFichier[LARGEUR * HAUTEUR + 1] = {0};
    int i = 0, j = 0;

    carte = fopen("carte.txt", "r");
    if (carte == NULL)
        return 0;

    fgets(ligneFichier, LARGEUR * HAUTEUR + 1, carte);

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            switch (ligneFichier[(i * LARGEUR) + j])
            {
                case '0':
                    tab_S_tile[j][i].terrain = NORMAL;
                    tab_S_tile[j][i].tank = VIDE;
                    break;
                case '1':
                    tab_S_tile[j][i].terrain = MINE;
                    tab_S_tile[j][i].tank = VIDE;
                    break;
                case '2':
                    tab_S_tile[j][i].terrain = POLLUE;
                    tab_S_tile[j][i].tank = VIDE;
                    break;
                case '3':
                    tab_S_tile[j][i].terrain = BASE1;
                    tab_S_tile[j][i].tank = TANK1;
                    break;
                case '4':
                    tab_S_tile[j][i].terrain = BASE2;
                    tab_S_tile[j][i].tank = TANK2;
                    break;
            }
        }
    }

    fclose(carte);
    tab_S_tile[0][0].tank = TANK1_CMD;
    tab_S_tile[10][10].tank = TANK2_CMD;

    for (i = 0; i<HAUTEUR ; i++)
    {
        for (j=0 ; j<LARGEUR ; j++)
        {
            tab_S_tile[i][j].autorise = 0;
        }
    }

    return 1;
}

void loadTextures( struct_textures *S_textures )
{
    /** Cette procédure charge toutes nos textures en mémoire.
     *
     *  Entrée: structure contenant les pointeurs vers les textures
     */

    S_textures->tex_terrain = IMG_LoadTexture( renderer, "./img/carte.png");
    S_textures->tex_tank[TANK1] = IMG_LoadTexture( renderer, "./img/tanka.png");
    S_textures->tex_tank[TANK2] = IMG_LoadTexture( renderer, "./img/tankb.png");
    S_textures->tex_tank[TANK1_CMD] = IMG_LoadTexture( renderer, "./img/commanda.png");
    S_textures->tex_tank[TANK2_CMD] = IMG_LoadTexture( renderer, "./img/commandb.png");
    S_textures->tex_highlight = IMG_LoadTexture( renderer, "./img/highlight2.png");
}

int menu()
{
    SDL_Texture *tex_titre = NULL, *tex_option1 = NULL, *tex_option2 = NULL;
    SDL_Surface *surf_texte = NULL;
    TTF_Font *police = NULL;
    SDL_Rect position = {0,0,0,0};
    SDL_Event event;
    SDL_Color noir = {0, 0, 0};

    int continuer = 0;

    police = TTF_OpenFont("FORCED_SQUARE.ttf", 65);
    surf_texte = TTF_RenderText_Blended(police, "TankWar", noir);
    tex_titre = SDL_CreateTextureFromSurface( renderer, surf_texte );

    police = TTF_OpenFont("FORCED_SQUARE.ttf", 35);
    surf_texte = TTF_RenderText_Blended(police, "Entr\xe9\x65\t- Commencer une nouvelle partie", noir);
    tex_option1 = SDL_CreateTextureFromSurface( renderer, surf_texte );

    surf_texte = TTF_RenderText_Blended(police, "Echap\t\t- Quitter le jeu", noir);
    tex_option2 = SDL_CreateTextureFromSurface( renderer, surf_texte );
    SDL_FreeSurface( surf_texte );

    SDL_RenderClear( renderer );


    SDL_QueryTexture(tex_titre, NULL, NULL, &position.w, &position.h);
    position.x = DIMENSION/2 - position.w/2;
    position.y = 100;
    SDL_RenderCopy( renderer, tex_titre, NULL, &position );


    SDL_QueryTexture(tex_option1, NULL, NULL, &position.w, &position.h);
    position.x = 50;
    position.y = 200;
    SDL_RenderCopy( renderer, tex_option1, NULL, &position );

    SDL_QueryTexture(tex_option2, NULL, NULL, &position.w, &position.h);
    position.x = 50;
    position.y = 250;
    SDL_RenderCopy( renderer, tex_option2, NULL, &position );

    SDL_RenderPresent( renderer );

    while (continuer == 0)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = -1;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: // Veut arrêter le jeu
                        continuer = -1;
                        break;
                    case SDLK_KP_1: // Demande à jouer
                        continuer = 1;
                        break;
                    case SDLK_RETURN:
                        continuer = 1;
                        break;
                    /*case SDLK_KP2:
                        editeur(ecran);
                        break;*/
                }
                break;
        }
    }

    TTF_CloseFont(police);
    TTF_Quit();
    SDL_DestroyTexture(tex_titre);
    SDL_DestroyTexture(tex_option1);
    SDL_DestroyTexture(tex_option2);

    return continuer;
}

void generateTextures( struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] )
{
    /** Cette procédure génère nos textures dans notre fenêtre de jeu
     *
     *  Entrée: structure contenant nos textures et structure contenant le tableau de jeu
     */

    SDL_Rect rect_tank = {0,0,BLOC,BLOC};
    SDL_Rect rect_carte = {0,0,DIMENSION,DIMENSION};
    int i,j;

    SDL_RenderCopy( renderer, S_textures->tex_terrain, NULL, &rect_carte );

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            rect_tank.y = i * BLOC;
            rect_tank.x = j * BLOC;
            switch(tab_S_tile[i][j].tank)
            {
                case 1:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK1], NULL, &rect_tank );
                    break;

                case 2:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK2], NULL, &rect_tank );
                    break;

                case 3:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK1_CMD], NULL, &rect_tank );
                    break;

                case 4:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK2_CMD], NULL, &rect_tank );
                    break;
            }

            if(tab_S_tile[i][j].autorise == 1)
            {
                SDL_RenderCopy( renderer, S_textures->tex_highlight, NULL, &rect_tank);
            }
        }
    }
}

void deplacerTank(int *joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] )
{
    /** Cette procédure sert à déplacer les tanks tour par tour
     *  Tant qu'on ne sort pas de la boucle infinie volontairement ou en fin de partie
     *
     *  Entrée: Joueur qui commence à jouer, notre structure de textures et notre structure contenant le terrain de jeu
     */

    int continuer = 1, mouvement = 0;
    int i,j;
    int x_tmp=0, y_tmp=0;
    struct_tank S_tank;
    S_tank.x=0;
    S_tank.y=0;
    S_tank.type=0;

    SDL_Event event;    // Utilisé pour la gestion des évènements
    SDL_Rect dest_mouvement = {0,0,BLOC,BLOC};  // Utilisé pour déplacer un tank et faire suivre la souris au sprite

    while (continuer)
    {
        SDL_WaitEvent(&event);

        // Si on quitte la fenêtre ou on appuie sur Echap, on sort de la boucle

        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
        {
            continuer = 0;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_GetMouseState( &x_tmp, &y_tmp );    // On récupère les coordonnées du clic
            printf("%d\n", *joueur);

            if (!mouvement)
            {
                S_tank.x = x_tmp/BLOC;
                S_tank.y = y_tmp/BLOC;
                S_tank.type = tab_S_tile[S_tank.y][S_tank.x].tank;

                if (*joueur == 1 && S_tank.type == TANK1 && tab_S_tile[S_tank.y][S_tank.x].terrain != BASE2)
                {
                    deplacementsPossibles( *joueur, tab_S_tile, S_tank);
                    mouvement = 1;
                }

                else if (*joueur == 1 && S_tank.type == TANK1_CMD && !(S_tank.x == 10 && S_tank.y == 10))
                {
                    deplacementsPossibles( *joueur, tab_S_tile, S_tank);
                    mouvement = 1;
                }

                else if (*joueur == 2 && S_tank.type == TANK2 && tab_S_tile[S_tank.y][S_tank.x].terrain != BASE1)
                {
                    deplacementsPossibles( *joueur, tab_S_tile, S_tank);
                    mouvement = 1;
                }

                else if (*joueur == 2 && S_tank.type == TANK2_CMD && !(S_tank.x==0 && S_tank.y==0))
                {
                    deplacementsPossibles( *joueur, tab_S_tile, S_tank);
                    mouvement = 1;
                }
            }

            else if (mouvement)
            {
                if (*joueur == 1 && tab_S_tile[y_tmp/BLOC][x_tmp/BLOC].autorise != INVALIDE)
                {
                    if(S_tank.type == TANK1)
                    {
                        tab_S_tile[y_tmp / BLOC][x_tmp / BLOC].tank = TANK1;
                    }

                    else tab_S_tile[y_tmp / BLOC][x_tmp / BLOC].tank = TANK1_CMD;

                    mouvement = 0;
                    *joueur = 2;
                    for (i = 0; i<HAUTEUR ; i++)
                    {
                    	for (j=0 ; j<LARGEUR ; j++)
                    	{
                    		tab_S_tile[i][j].autorise = 0;
                    	}
                    }
                }

                else if (*joueur == 2 && tab_S_tile[y_tmp/BLOC][x_tmp/BLOC].autorise != INVALIDE)
                {
                    if(S_tank.type == TANK2)
                    {
                        tab_S_tile[y_tmp / BLOC][x_tmp / BLOC].tank = TANK2;
                    }

                    else tab_S_tile[y_tmp / BLOC][x_tmp / BLOC].tank = TANK2_CMD;

                    mouvement = 0;
                    *joueur = 1;
                    for (i = 0; i<HAUTEUR ; i++)
                    {
                    	for (j=0 ; j<LARGEUR ; j++)
                    	{
                    		tab_S_tile[i][j].autorise = 0;
                    	}
                    }
                }
            }
        }

        // Quand on a sélectionné un tank et on bouge la souris, le tank suit nos mouvements

        if (event.type == SDL_MOUSEMOTION && mouvement)
        {
            dest_mouvement.x = event.motion.x;
            dest_mouvement.y = event.motion.y;
        }

        SDL_RenderClear( renderer );
        generateTextures( S_textures, tab_S_tile );

        if (mouvement && *joueur == 1)
        {
            if (S_tank.type == TANK1)
            {
                SDL_RenderCopy( renderer, S_textures->tex_tank[TANK1], NULL, &dest_mouvement );
            }

            else
            {
                SDL_RenderCopy( renderer, S_textures->tex_tank[TANK1_CMD], NULL, &dest_mouvement );
            }
        }

        if (mouvement && *joueur == 2)
        {
            if (S_tank.type == TANK2)
            {
                SDL_RenderCopy( renderer, S_textures->tex_tank[TANK2], NULL, &dest_mouvement );
            }

            else
            {
                SDL_RenderCopy( renderer, S_textures->tex_tank[TANK2_CMD], NULL, &dest_mouvement );
            }
        }

        SDL_RenderPresent( renderer );
    }
}

void deplacementsPossibles( int joueur, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank )
{
    /** Cette procédure étudie au cas par cas les possibilités de déplacements dans les 8 directions possibles
     *  La gestion des collisions au sein de chaque boucle est déléguée à collision()
     *
     *  Les déplacements possibles sont représentés par un "1" dans le tableau deplacement_possibles
     */

    int i,j;
    tab_S_tile[S_tank.y][S_tank.x].autorise = 1;    // On peut reposer le tank sur sa position de départ

    /* VERS LE BAS */

    for (i=S_tank.y+1; i<HAUTEUR; i++)
    {
        if (collision( joueur, i, S_tank.x, tab_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* VERS LE HAUT */

    for (i=S_tank.y-1; i>=0; i--)
    {
        if (collision( joueur, i, S_tank.x, tab_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* VERS LA GAUCHE */

    for (i=S_tank.x-1; i>=0; i--)
    {
        if (collision( joueur, S_tank.y, i, tab_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* VERS LA DROITE */

    for (i=S_tank.x+1; i<LARGEUR; i++)
    {
        if (collision( joueur, S_tank.y, i, tab_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* Déplacement en diagonale vers la droite et vers le bas de la position actuelle */

    /*if((terrain->tab_terrain[S_tank.y+1][S_tank.x] == NORMAL && terrain->tab_terrain[S_tank.y][S_tank.x+1] == NORMAL)
       || (terrain->tab_terrain[S_tank.y+1][S_tank.x] == POLLUE && terrain->tab_terrain[S_tank.y][S_tank.x+1] == POLLUE))
    {*/
        for (i=S_tank.y+1, j=S_tank.x+1; i<HAUTEUR && j<LARGEUR; i++,j++)
        {
            if (collision( joueur, i, j, tab_S_tile, S_tank) == 0)
            {
                break;
            }
        }


    /* Déplacement en diagonale vers la droite et vers le haut de la position actuelle */

        for (i=S_tank.y-1, j=S_tank.x+1; i>=0 && j<LARGEUR; i--,j++)
        {
            if (collision( joueur,i, j, tab_S_tile, S_tank) == 0)
            {
                break;
            }
        }

    /* Déplacement en diagonale vers la gauche et vers le bas de la position actuelle */

        for (i=S_tank.y+1, j=S_tank.x-1; i<HAUTEUR && j>=0; i++,j--)
        {
            if (collision( joueur, i, j, tab_S_tile, S_tank) == 0)
            {
                break;
            }
        }

    /* Déplacement en diagonale vers la gauche et vers le haut de la position actuelle */

        for (i=S_tank.y-1, j=S_tank.x-1; i>=0 && j>=0; i--,j--)
        {
            if (collision( joueur, i, j, tab_S_tile, S_tank) == 0)
            {
                break;
            }
        }

        tab_S_tile[S_tank.y][S_tank.x].tank = VIDE;

}

int collision(int joueur, int i, int j, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank)
{
    /** Cette fonction gère les collisions dans une direction, fournie par deplacementsPossibles()
     *
     *  On vérifie le joueur, la position initiale du tank (y,x) et le type de tank sélectionné
     *  i et j sont les compteurs de chaque boucle dans deplacementsPossibles().
     *  L'un des compteurs peut être remplacé par x ou y selon la direction étudiée
     *
     *  On retourne 0 si on est arrêté par une case interdite afin de sortir de la boucle
     */

    if ((joueur == 1 && (tab_S_tile[i][j].tank == TANK1 || tab_S_tile[i][j].tank == TANK1_CMD))
        || (joueur == 2 && (tab_S_tile[i][j].tank == TANK2 || tab_S_tile[i][j].tank == TANK2_CMD))
        || ( tab_S_tile[i][j].terrain == MINE ))
    {
        return 0;
    }

    else if (tab_S_tile[i][j].terrain == POLLUE && (S_tank.type == TANK1 || S_tank.type == TANK2))
    {
        return 0;
    }

    else if ((joueur == 1 && (tab_S_tile[i][j].tank == TANK2 || tab_S_tile[i][j].tank == TANK2_CMD))
             || (joueur == 2 && (tab_S_tile[i][j].tank == TANK1 || tab_S_tile[i][j].tank == TANK1_CMD)))
    {
        if (tab_S_tile[i][j].terrain == POLLUE)
        {
            return 0;
        }

        else
        {
           tab_S_tile[i][j].autorise = 1;
            return 0;
        }
    }

    else
    {
        tab_S_tile[i][j].autorise = 1;
    }

    return 1;
}

void pause()
{
    /**  Cette procédure permet de pauser le programme quand il a finit d'executer toutes les instructions.
     *   Cela laisse le temps à l'utilisateur d'intéragir avec le jeu et de quitter le jeu par lui-même
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

void close( struct_textures *textures )
{
    /** Cette procédure vide la mémoire, pointe nos pointeurs sur NULL et quitte SDL
     *
     *  Entrée: structure contenant nos textures
     */

    int i;

    for (i = 0 ; i < 5 ; i++)
    {
        SDL_DestroyTexture( textures->tex_tank[i] );
        textures->tex_tank[i] = NULL;
    }
    SDL_DestroyTexture( textures->tex_highlight );
    textures->tex_highlight = NULL;
    SDL_DestroyTexture( textures->tex_terrain );
    textures->tex_terrain = NULL;
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( ecran );
    renderer= NULL;
    ecran = NULL;
    IMG_Quit();
    SDL_Quit();
}
