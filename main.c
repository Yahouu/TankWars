/** Projet de programmation en C - 1ère année
 *  TankWar
 *  Construit avec SDL 2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

// On définit notre tableau de jeu

#define BLOC                65                  // Dimensions en pixels d'un bloc
#define DIMTANK             64                  // Dimensions d'un tank
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           716                 // Dimensions de notre carte

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
    SDL_Texture *tex_terrain;
    SDL_Texture *tex_highlight;
    SDL_Texture *tex_tank[5];
    SDL_Texture *tex_joueur[2];
    SDL_Texture *tex_menu_bg;
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
void writeText(const char* text, SDL_Surface *surf_texte, SDL_Texture *tex_texte, TTF_Font *police, SDL_Rect *position, int x, int y);
int menu(struct_textures *S_textures);
void sauvegarder( int joueur, struct_tile tab_S_tile[][LARGEUR] );
void chargerPartie( int *joueur, struct_tile tab_S_tile[][LARGEUR] );
void lancement( int *joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] );
void generateTextures( int joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] );
void endGame( int score1, int score2);
int checkEndGame( struct_tile tab_S_tile[][LARGEUR] );
void deplacerTank(int *joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] );
int collision(int joueur, int i, int j, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank);
void deplacementsPossibles( int joueur, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank );
void close( struct_textures *S_textures );

// Fenêtre de jeu et renderer

SDL_Window *ecran = NULL;
SDL_Renderer *renderer = NULL;
Mix_Music *musique = NULL;

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
    lancement( &joueur, &S_textures, tab_S_tile );

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

    if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
    {
        fprintf(stderr,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        init=0;
    }

    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        fprintf(stderr, "Échec de l'initialisation du filtrage linéaire de textures!" );
    }

    // Création de la fenêtre

    ecran = SDL_CreateWindow("TankWar", SDL_WINDOWPOS_UNDEFINED,    // Position de la fenêtre sur l'écran de l'utilisateur
                                        SDL_WINDOWPOS_UNDEFINED,    // Ici ça ne nous importe pas
                                        DIMENSION,                  // Dimensions
                                        DIMENSION,
                                        SDL_WINDOW_SHOWN);          // On affiche la fenêtre, cachée par défaut

    if( ecran == NULL )
    {
        fprintf( stderr, "Échec de la création de la fenêtre: %s\n", SDL_GetError() );
        init = 0;
    }

    // Création du renderer

    renderer = SDL_CreateRenderer( ecran, -1, SDL_RENDERER_ACCELERATED );

    if( ecran == NULL )
    {
        fprintf( stderr, "Échec de la création du renderer: SDL Error: %s\n", SDL_GetError() );
        init = 0;
    }

    // La couleur par défaut dans la fenêtre est le blanc

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // Initialisation de SDL_Image

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stderr,"Echec de l'initialisation du support des png et jpg par IMG_Init (%s)\n",IMG_GetError());
        init=0;
    }

    // Initialisation de SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        fprintf(stderr, "Echec de l'initialisation de SDL_mixer. Erreur: %s\n", Mix_GetError() );
        init = 0;
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
    /** Cette fonction génère notre carte au début du jeu et place les tanks.
     *  On met aussi à zéro toutes les possibilités de déplacement
     *
     *  Elle retourne 1 si elle est exécutée jusqu'au bout.
     */

    FILE* carte = NULL;
    char ligne[LARGEUR * HAUTEUR + 1] = {0};
    int i = 0, j = 0;

    carte = fopen("carte.txt", "r");
    if (carte == NULL)
        return 0;

    fgets(ligne, LARGEUR * HAUTEUR + 1, carte);

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            switch (ligne[(i * LARGEUR) + j])
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
    /**
     *  Cette procédure charge toutes nos textures en mémoire.
     */

    TTF_Font *police = NULL;
    police = TTF_OpenFont("FORCED_SQUARE.ttf", 25);
    SDL_Color noir = {0, 0, 0};

    S_textures->tex_terrain = IMG_LoadTexture( renderer, "./img/carte.png");
    S_textures->tex_tank[TANK1] = IMG_LoadTexture( renderer, "./img/tanka.png");
    S_textures->tex_tank[TANK2] = IMG_LoadTexture( renderer, "./img/tankb.png");
    S_textures->tex_tank[TANK1_CMD] = IMG_LoadTexture( renderer, "./img/commanda.png");
    S_textures->tex_tank[TANK2_CMD] = IMG_LoadTexture( renderer, "./img/commandb.png");
    S_textures->tex_highlight = IMG_LoadTexture( renderer, "./img/highlight2.png");
    S_textures->tex_menu_bg = IMG_LoadTexture( renderer, "./img/menu_bg.png");
    S_textures->tex_joueur[0] = SDL_CreateTextureFromSurface( renderer, TTF_RenderText_Blended(police, "Joueur 1", noir) );
    S_textures->tex_joueur[1] = SDL_CreateTextureFromSurface( renderer, TTF_RenderText_Blended(police, "Joueur 2", noir) );

    TTF_CloseFont(police);
}

void lancement( int *joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] )
{
    /** Cette procédure gère le lancement du jeu.
     *  Selon le choix du jour dans le menu, on effectue diverses actions.
     *
     *  Si le joueur quitte, on sauvegarde le plateau de jeu et on quitte le jeu.
     *  Si le joueur ouvre le jeu et continue sa partie, on la charge.
     *  Si le joueur commence une nouvelle partie en cours de jeu, on regenère le terrain.
     *
     *  Quand le joueur sort de la boucle principale du jeu, on appelle cette procédure de nouveau par récursion
     */

    static int i = 0;  // Ce compteur nous permet de savoir si on vient de lancer le jeu ou non
    int choix = menu(S_textures);

    if (choix == -1)
    {
        sauvegarder( *joueur, tab_S_tile );
        close( S_textures );
    }

    else
    {
        if (choix == 1 && i == 0)
        {
            chargerPartie( joueur, tab_S_tile );
        }

        if (choix == 2 && i != 0)
        {
            genererCarte( tab_S_tile );
        }

        SDL_RenderClear( renderer );
        generateTextures( *joueur, S_textures, tab_S_tile );
        SDL_RenderPresent( renderer );

        // Boucle infinie pour bouger un tank
        deplacerTank( joueur, S_textures, tab_S_tile );

        ++i;
        lancement(joueur, S_textures, tab_S_tile);
    }
}

int menu(struct_textures *S_textures)
{
    /** Cette fonction génère notre menu
     *
     *  Le joueur a trois options:
     *  - Continuer une partie
     *  - Commencer une nouvelle partie
     *  - Quitter le jeu
     */

    SDL_Texture *tex_titre = NULL, *tex_option1 = NULL, *tex_option2 = NULL, *tex_option3 = NULL, *tex_option4 = NULL;
    SDL_Surface *surf_texte = NULL;
    TTF_Font *police = NULL;
    SDL_Rect position = {0,0,DIMENSION,DIMENSION};
    SDL_Event event;

    musique = Mix_LoadMUS( "PurpleSwag.mp3" );
    if( musique == NULL )
	{
		fprintf( stderr, "Echec du chargement de la musique! Erreur SDL_mixer: %s\n", Mix_GetError() );
	}

    int continuer = 0;

    SDL_RenderClear( renderer );
    SDL_RenderCopy( renderer, S_textures->tex_menu_bg, NULL, &position );

    police = TTF_OpenFont("FORCED_SQUARE.ttf", 65);
    writeText("TankWar", surf_texte, tex_option1, police, &position, (DIMENSION/2 - 130), 100);

    police = TTF_OpenFont("FORCED_SQUARE.ttf", 35);
    writeText("Entr\xe9\x65\t- Continuer une partie", surf_texte, tex_option1, police, &position, 50, 200);
    writeText("N        - Nouvelle partie", surf_texte, tex_option1, police, &position, 50, 250);
    writeText("M        - Jouer/Pauser la musique", surf_texte, tex_option1, police, &position, 50, 300);
    writeText("Echap  - Quitter le jeu", surf_texte, tex_option1, police, &position, 50, 350);

    SDL_FreeSurface( surf_texte );
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
                    case SDLK_RETURN: // Charger une partie
                        continuer = 1;
                        break;
                    case SDLK_n:    // Lancer une nouvelle partie
                        continuer = 2;
                        break;
                    case SDLK_m:
                        //Si la musique n'est pas en train de jouer...
                        if( Mix_PlayingMusic() == 0 )
                        {
                            //... on lance la musique
                            Mix_PlayMusic( musique, -1 );
                        }
                        //Si elle est déjà lancée...
                        else
                        {
                            //... mais si elle est en pause ...
                            if( Mix_PausedMusic() == 1 )
                            {
                                //... on la relance.
                                Mix_ResumeMusic();
                            }
                            //... et qu'elle est en train d'être jouée.
                            else
                            {
                                //... on la pause.
                                Mix_PauseMusic();
                            }
                        }
                        break;
                }
                break;
        }
    }

    TTF_CloseFont(police);
    SDL_DestroyTexture(tex_titre);
    SDL_DestroyTexture(tex_option1);
    SDL_DestroyTexture(tex_option2);
    SDL_DestroyTexture(tex_option3);
    SDL_DestroyTexture(tex_option4);

    return continuer;
}

void writeText(const char* text, SDL_Surface* surf_texte, SDL_Texture* tex_texte, TTF_Font* police, SDL_Rect* position, int x, int y)
{
    SDL_Color noir = {0, 0, 0};
    surf_texte = TTF_RenderText_Blended(police, text, noir);
    tex_texte = SDL_CreateTextureFromSurface( renderer, surf_texte );

    SDL_QueryTexture(tex_texte, NULL, NULL, &position->w, &position->h);
    position->x = x;
    position->y = y;
    SDL_RenderCopy( renderer, tex_texte, NULL, position );
}

void chargerPartie( int *joueur, struct_tile tab_S_tile[][LARGEUR])
{
    /**
     *  Cette procédure charge la partie sauvegardée si le joueur veut la continuer
     */

    FILE* sauvegarde = NULL;
    char ligne[LARGEUR * HAUTEUR + 2] = {0};    // +2 afin de lire le joueur sauvegardé et pour le caractère de fin de chaîne
    int i = 0, j = 0;

    sauvegarde = fopen("sauvegarde.txt", "r");
    if (sauvegarde == NULL)
        fprintf(stderr, "Le chargement a échoué, une nouvelle partie recommence");

    fgets(ligne, LARGEUR * HAUTEUR + 2, sauvegarde);

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            switch (ligne[(i * LARGEUR) + j])
            {
                case '0':
                    tab_S_tile[j][i].tank = VIDE;
                    break;
                case '1':
                    tab_S_tile[j][i].tank = TANK1;
                    break;
                case '2':
                    tab_S_tile[j][i].tank = TANK2;
                    break;
                case '3':
                    tab_S_tile[j][i].tank = TANK1_CMD;
                    break;
                case '4':
                    tab_S_tile[j][i].tank = TANK2_CMD;
                    break;
            }
        }
    }

    *joueur = ligne[LARGEUR*HAUTEUR] - '0';    // Converti notre char en int

    fclose(sauvegarde);
}

void generateTextures( int joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] )
{
    /**
     *  Cette procédure génère nos textures dans notre fenêtre de jeu
     */

    SDL_Rect rect_tank = {0,0,BLOC,BLOC};
    SDL_Rect rect_carte = {0,0,DIMENSION,DIMENSION};
    SDL_Rect rect_text = {0,0,0,0};

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

    SDL_QueryTexture(S_textures->tex_joueur[1], NULL, NULL, &rect_text.w, &rect_text.h);
    rect_text.x = DIMENSION - rect_text.w*1.2;
    rect_text.y = 0.2*rect_text.h;
    if(joueur == 1)
    {
        SDL_RenderCopy( renderer, S_textures->tex_joueur[0], NULL, &rect_text );
    }

    else SDL_RenderCopy( renderer, S_textures->tex_joueur[1], NULL, &rect_text );
}

void deplacerTank(int *joueur, struct_textures *S_textures, struct_tile tab_S_tile[][LARGEUR] )
{
    /** Cette procédure sert à déplacer les tanks tour par tour
     *  Tant qu'on ne sort pas de la boucle infinie volontairement ou en fin de partie
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

        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            continuer = 0;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_GetMouseState( &x_tmp, &y_tmp );    // On récupère les coordonnées du clic
            if (!mouvement)
            {
                S_tank.x = x_tmp/BLOC;
                S_tank.y = y_tmp/BLOC;
                dest_mouvement.x = x_tmp;
                dest_mouvement.y = y_tmp;
                S_tank.type = tab_S_tile[S_tank.y][S_tank.x].tank;

                if ((*joueur == 1 && (S_tank.type == TANK1 || S_tank.type == TANK1_CMD))
                    || (*joueur == 2 && (S_tank.type == TANK2 || S_tank.type == TANK2_CMD)))
                {
                    deplacementsPossibles( *joueur, tab_S_tile, S_tank);
                    mouvement = 1;
                }
            }

            else if (mouvement)
            {
                if (*joueur == 1 && tab_S_tile[y_tmp/BLOC][x_tmp/BLOC].autorise != INVALIDE)
                {
                    /* On place soit un tank normal, soit un tank commandant */
                    if(S_tank.type == TANK1)
                    {
                        tab_S_tile[y_tmp / BLOC][x_tmp / BLOC].tank = TANK1;
                    }

                    else tab_S_tile[y_tmp / BLOC][x_tmp / BLOC].tank = TANK1_CMD;

                    /* On sort de la phase de mouvement et on change de joueur */
                    mouvement = 0;
                    *joueur = 2;

                    /* On réinitialise les possibilités de déplacement */
                    for (i = 0; i<HAUTEUR ; i++)
                    {
                    	for (j=0 ; j<LARGEUR ; j++)
                    	{
                    		tab_S_tile[i][j].autorise = 0;
                    	}
                    }

                    /* On régénère les textures */
                    SDL_RenderClear( renderer );
                    generateTextures( *joueur, S_textures, tab_S_tile );
                    SDL_RenderPresent( renderer );

                    /* On vérifie que la partie n'est pas terminée */
                    continuer = checkEndGame(tab_S_tile);
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

                    SDL_RenderClear( renderer );
                    generateTextures( *joueur, S_textures, tab_S_tile );
                    SDL_RenderPresent( renderer );

                    continuer = checkEndGame(tab_S_tile);
                }
            }
        }

        if (mouvement)
        {
            // Quand on a sélectionné un tank et on bouge la souris, le tank suit nos mouvements
            if (event.type == SDL_MOUSEMOTION)
            {
                dest_mouvement.x = event.motion.x;
                dest_mouvement.y = event.motion.y;
            }

            SDL_RenderClear( renderer );
            generateTextures( *joueur, S_textures, tab_S_tile );

            if (*joueur == 1)
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

            else if (*joueur == 2)
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

}

void deplacementsPossibles( int joueur, struct_tile tab_S_tile[][LARGEUR], struct_tank S_tank )
{
    /** Cette procédure étudie au cas par cas les possibilités de déplacements dans les 8 directions possibles
     *  La gestion des collisions au sein de chaque boucle est déléguée à collision()
     *
     *  Chaque case du tableau de jeu est affectée d'une valeur binaire si l'on peut se déplacer dessus ou non
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

    /* Si on rencontre un tank allié ou une mine, on s'arrête */
    if ((joueur == 1 && (tab_S_tile[i][j].tank == TANK1 || tab_S_tile[i][j].tank == TANK1_CMD))
        || (joueur == 2 && (tab_S_tile[i][j].tank == TANK2 || tab_S_tile[i][j].tank == TANK2_CMD))
        || ( tab_S_tile[i][j].terrain == MINE ))
    {
        return 0;
    }

    /* Si on rencontre une case polluée avec un tank normal, on s'arrête */
    else if (tab_S_tile[i][j].terrain == POLLUE && (S_tank.type == TANK1 || S_tank.type == TANK2))
    {
        return 0;
    }

    /* Si on est dans la base enemie, on ne peut plus en resortir */
    else if ((joueur == 1 && tab_S_tile[S_tank.y][S_tank.x].terrain == BASE2 && tab_S_tile[i][j].terrain == NORMAL)
             || (joueur == 2 && tab_S_tile[S_tank.y][S_tank.x].terrain == BASE1 && tab_S_tile[i][j].terrain == NORMAL))
    {
        return 0;
    }

    /* Si on rencontre un tank enemi, on peut le capturer et ensuite on s'arrête, sauf s'il est dans une case polluée */
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

    /* Sinon, on peut se déplacer */
    else
    {
        tab_S_tile[i][j].autorise = 1;
    }

    return 1;
}

int checkEndGame( struct_tile tab_S_tile[][LARGEUR] )
{
    /** Cette procédure vérifie si la partie est terminée ou non, puis compte les points si nécessaire.
     *  La partie se termine si:
     *
     *  - Un des deux joueurs n'a plus de tanks
     *  - Tous les tanks d'un joueur sont dans la base enemie
     */

    int i, j, count1 = 0, count2 = 0, frozen1 = 0, frozen2 = 0, cmd1 = 0, cmd2 = 0, fin = 0, score1 = 0, score2 = 0;

    /* On compte le nombre de tanks restant et ceux qui ne peuvent plus sortir de la base */

    for ( i=0 ; i < LARGEUR ; i++)
    {
        for ( j=0 ; j < HAUTEUR ; j++)
        {
            if (tab_S_tile[j][i].tank == TANK1 || tab_S_tile[j][i].tank == TANK1_CMD)
            {
                ++count1;
                if (tab_S_tile[j][i].tank == TANK1 && tab_S_tile[j][i].terrain == BASE2)
                {
                    ++frozen1;
                }

                /* On veut savoir si le tank commandant est dans la base ennemie mais pas à la place du commandant ennemi */
                if ((i!=0 || j!=0) && tab_S_tile[j][i].tank == TANK1_CMD && tab_S_tile[j][i].terrain == BASE2)
                {
                    ++cmd1;
                }
            }

            else if (tab_S_tile[j][i].tank == TANK2 || tab_S_tile[j][i].tank == TANK2_CMD)
            {
                ++count2;
                if (tab_S_tile[j][i].tank == TANK2 && tab_S_tile[j][i].terrain == BASE1)
                {
                    ++frozen2;
                }

                if ((i!=10 || j!=10) && tab_S_tile[j][i].tank == TANK2_CMD && tab_S_tile[j][i].terrain == BASE1)
                {
                    ++cmd2;
                }
            }
        }
    }

    if (tab_S_tile[0][0].tank == TANK2_CMD)
    {
        ++frozen2;
    }

    if (tab_S_tile[10][10].tank == TANK1_CMD)
    {
        ++frozen1;
    }

    /* On vérifie si la partie doit se terminer */

    if (count1 == 0 || count2 == 0)
    {
        fin = 1;
    }

    else if (frozen1 == count1 || frozen2 == count2)
    {
        fin = 1;
    }

    /* On compte les points si la partie est terminée */

    if (fin == 1)
    {
        /*  On commence par vérifier si le char du commandant se trouve sur la case de départ du char commandant adverse.
            Si le tank commandant est dans la base ennemie mais pas sur la case de départ du commandant ennemi,
            il compte pour deux points, comme un tank normal "gelé" */

        if (tab_S_tile[0][0].tank == TANK2_CMD)
        {
            score2 += 3;
            --frozen2;
            --count2;
        }

        else if (cmd2 == 1)
        {
            score2 += 2;
            --count2;
        }

        if (tab_S_tile[10][10].tank == TANK1_CMD)
        {
            score1 += 3;
            --frozen1;
            --count1;
        }

        else if (cmd1 == 1)
        {
            score1 += 2;
            --count1;
        }

        /* On continue avec les chars qui se trouvent dans la base ennemie */

        for (  ; frozen2 > 0 ; frozen2--, count2--)
        {
            score2 += 2;
        }

        for (  ; frozen1 > 0 ; frozen1--, count1--)
        {
            score1 += 2;
        }

        /* On fini par les tanks sur le reste de la carte, s'il en reste */

        for (  ; count2 > 0 ; count2--)
        {
            score2 += 1;
        }

        for (  ; count1 > 0 ; count1--)
        {
            score1 += 1;
        }

        /* On termine la partie */

        endGame( score1, score2);
        genererCarte( tab_S_tile ); //  On réinitialise le terrain de jeu
        return 0;
    }

    return 1;
}

void endGame( int score1, int score2)
{
    /** Cette procédure gère la fin du jeu.
     *  On affiche les scores ainsi que le gagnant.
     */

    int continuer = 1;
    SDL_Event event;
    SDL_Texture *tex_game_over = NULL, *tex_score1 = NULL, *tex_score2 = NULL;
    SDL_Surface *surf_texte = NULL;
    TTF_Font *police = NULL;
    SDL_Rect position = {0,0,0,0};
    SDL_Color noir = {0, 0, 0};

    police = TTF_OpenFont("FORCED_SQUARE.ttf", 55);
    char s_score1[20], s_score2[20];
    sprintf(s_score1, "Joueur 1: %d points", score1);   // On convertit le score entier en string
    sprintf(s_score2, "Joueur 2: %d points", score2);

    SDL_RenderClear( renderer );

    if (score2 > score1)
    {
        surf_texte = TTF_RenderText_Blended(police, "Joueur 2 a gagn\xe9 la partie !", noir);
    }

    else if (score1 > score2)
    {
        surf_texte = TTF_RenderText_Blended(police, "Joueur 1 a gagn\xe9 la partie !", noir);
    }

    else
    {
        surf_texte = TTF_RenderText_Blended(police, "Les joueurs sont ex-aequo !", noir);
    }

    // On n'utilise pas writeText car on doit pouvoir adapter sa position en fonction de sa largeur pour centrer le texte
    tex_game_over = SDL_CreateTextureFromSurface( renderer, surf_texte );
    SDL_QueryTexture(tex_game_over, NULL, NULL, &position.w, &position.h);
    position.x = DIMENSION/2 - position.w/2;
    position.y = 100;
    SDL_RenderCopy( renderer, tex_game_over, NULL, &position );

    police = TTF_OpenFont("FORCED_SQUARE.ttf", 35);
    writeText(s_score1, surf_texte, tex_score1, police, &position, 50, 200);
    writeText(s_score2, surf_texte, tex_score2, police, &position, 50, 250);

    SDL_FreeSurface( surf_texte );
    SDL_RenderPresent( renderer );

    while (continuer)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE)
        {
            continuer = 0;
        }
    }

    TTF_CloseFont(police);
    SDL_DestroyTexture(tex_game_over);
    SDL_DestroyTexture(tex_score1);
    SDL_DestroyTexture(tex_score2);
}

void sauvegarder( int joueur, struct_tile tab_S_tile[][LARGEUR] )
{
    /**
     *  Cette procédure sauvegarde notre plateau de jeu et le dernier joueur dans un fichier sauvegarde.txt
     */

    FILE* sauvegarde = NULL;
    int i = 0, j = 0;

    sauvegarde = fopen("sauvegarde.txt", "w");
    if (sauvegarde == NULL)
        fprintf(stderr, "Echec de la sauvegarde\n");

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            fprintf(sauvegarde, "%d", tab_S_tile[j][i].tank);
        }
    }

    fprintf(sauvegarde, "%d", joueur);

    fclose(sauvegarde);
}

void close( struct_textures *S_textures )
{
    /**
     *  Cette procédure vide la mémoire, pointe nos pointeurs sur NULL et quitte SDL
     */

    int i;

    for (i = 1 ; i < 5 ; i++)
    {
        SDL_DestroyTexture( S_textures->tex_tank[i] );
        S_textures->tex_tank[i] = NULL;
    }

    for (i = 0 ; i < 2 ; i++)
    {
        SDL_DestroyTexture( S_textures->tex_joueur[i] );
        S_textures->tex_joueur[i] = NULL;
    }

    SDL_DestroyTexture( S_textures->tex_highlight );
    S_textures->tex_highlight = NULL;
    SDL_DestroyTexture( S_textures->tex_terrain );
    S_textures->tex_terrain = NULL;
    SDL_DestroyTexture( S_textures->tex_menu_bg );
    S_textures->tex_menu_bg = NULL;
    Mix_FreeMusic( musique );
    musique = NULL;
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( ecran );
    renderer= NULL;
    ecran = NULL;
    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}
