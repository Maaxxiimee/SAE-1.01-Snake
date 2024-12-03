#include <fcntl.h> // Definition des librairies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

#define TAILLE 20 // Taille du serpent
#define LONGUEUR 80
#define LARGEUR 40

int tabX[TAILLE], tabY[TAILLE]; // Déclaration des variables
const char TETE = 'O'; // Tete du serpent
const char CORP = 'X'; // Corp du serpent
const char ARRET = 'a'; // Touche d'arret du programme
const char HAUT = 'z'; // Déclaration des variables concernant le controle du serpent
const char BAS = 's';
const char GAUCHE = 'q';
const char DROITE = 'd';
const char BORDURE = '#'; // Déclaration des variables pour la structure du tableau
const char VIDE = ' ';
const char POMME = '6';
const int NBBLOCS = 4;
const int TAILLEPAVE = 5;
const int POS_INIT_X = 40;
const int POS_INIT_Y = 20;

typedef char t_plateau[LONGUEUR+1][LARGEUR+1];  // Définition d'un nouveau type t_plateau représentant un tableau à double dimension

void gotoXY(int x, int y); // Entête des différentes fonctions
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[], int pommeMangee);
void progresser(int lesX[], int lesY[], char direction, t_plateau plato, bool * contact, bool * contactPomme, int pommeMangee);
void disableEcho();
void enableEcho();
int kbhit();
char kbhitChar();
void initPlateau(t_plateau plato);
void dessinerPlateau(t_plateau plato);
void ajouterPomme(t_plateau plato);

/***************************************************
 *                                                 *
 *          Début du programme principal (main)    *
 *                                                 *
 ***************************************************/

int main() {
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires

    int delai = 150000; // Délai de 150 millisecondes
	char touche = DROITE; // Direction par défaut du serpent
    char nouvelleTouche; // Stockage temporaire de la touche entrée
    bool contactSerpent = 0; // Booléen indiquant que le serpent est en contact ( 1 ) ou non ( 0 )
    bool contactPomme = 0;
    int pommeMangee = 0;
    t_plateau tabPlateau;

    tabX[0] = POS_INIT_X; // Initialisation des deux tableaux contenant les coordonées du serpent
    tabY[0] = POS_INIT_Y;
    for (int i = 1; i < TAILLE+1; i++) {
        tabX[i] = POS_INIT_X - i;
        tabY[i] = POS_INIT_Y;
    }

    system("clear"); // Nettoyage du contenu du terminal
    disableEcho();  // Désactive l'affichage des touches entrées au clavier
    printf("\033[?25l"); // Enlève le curseur afin d'éviter un bug d'affichage lorsque le serepent disparait

    
    initPlateau(tabPlateau); // Initialisation du plateau
    ajouterPomme(tabPlateau);
    dessinerPlateau(tabPlateau); // Affichage du plateau

    while (touche!=ARRET && !contactSerpent && pommeMangee!=10) { // Démarrage de la boucle while / du mouvement du serpent
        disableEcho(); // désactive l'affichage dans le terminal des caractères entrés
        dessinerSerpent(tabX, tabY, pommeMangee);
        usleep(delai); // delai de 90ms entre chaque mise à jour de l'avancée du serpent
        progresser(tabX, tabY, touche, tabPlateau ,&contactSerpent, &contactPomme, pommeMangee); 

        if(contactPomme && pommeMangee<=9) {
            pommeMangee++;
            gotoXY(1,1);           
            ajouterPomme(tabPlateau);
            dessinerPlateau(tabPlateau);
            delai+=-12000;
            contactPomme=false; 
        } 
        if(pommeMangee==10) {
            printf("Félicitations ! Vous avez mangé 10 pommes !\n");
        }

        if(kbhit()) { // Test des conditions afin de détecter si la toiche pressé est connue et si elle n'est pas l'opposé de la touche actuelle
            scanf("%c", &nouvelleTouche);
            if ((nouvelleTouche == HAUT || nouvelleTouche == BAS || nouvelleTouche == GAUCHE || nouvelleTouche == DROITE || nouvelleTouche == ARRET) && 
            !(touche==DROITE && nouvelleTouche==GAUCHE) && !(touche==GAUCHE && nouvelleTouche==DROITE) && !(touche==HAUT && nouvelleTouche==BAS) && !(touche==BAS && nouvelleTouche==HAUT)) {
                touche = nouvelleTouche;
            }
        }
    }
    printf("\033[?25h"); // Ré-active le curseur
    enableEcho(); // Ré-active l'echo
    return 0;
}


void gotoXY(int x, int y) { // Déplace le curseur a une position (x,y) 
    printf("\033[%d;%df", y, x); 
}

void afficher(int x, int y, char c) { // Affiche un caractère fournis en paramètre selon la position x;y voulue
	gotoXY(x, y);
	printf("%c", c);
}

void effacer(int x, int y) {  // Affiche (efface) un espace " " fournis en paramètre selon la position x;y voulue
	gotoXY(x, y);
	printf(" ");
}

void dessinerSerpent(int lesX[], int lesY[], int pommeMangee) { // Dessine le serpent. Prend maintenant en compte la position de la tête afin de l'afficher ou non
    int i;                             

    for (i = 0; i < TAILLE-10+1; i++) {
        effacer(lesX[i], lesY[i]);
    }

    for (i = 1; i < TAILLE-10+1; i++) {
        if (lesX[i] >= 0 && lesY[i] >= 0) {
            afficher(lesX[i], lesY[i], CORP);
        }
    }

    if (lesX[0] >= 0 && lesY[0] >= 0) {
        afficher(lesX[0], lesY[0], TETE);
    }
}

void progresser(int lesX[], int lesY[], char direction, t_plateau plato, bool * contact, bool * contactPomme, int pommeMangee) { // Progresse le serpent de sa position vers une direction donnée, vérifie sa position avec les obstacles
    effacer(lesX[TAILLE-10+pommeMangee], lesY[TAILLE-10+pommeMangee]); // Positionne tous les éléments du serpent à une position x+1 en effacant le dernier élément afin d'avoir seulement 9 anneaux
    for(int i = TAILLE-10+pommeMangee+1 - 1; i > 0; i--) {
        lesX[i] = lesX[i-1];
        lesY[i] = lesY[i-1];
    }
    if(direction==DROITE) { // Prise en compte du caractère pressé fournis en paramètre afin de diriger les prochains caractères dans la direction voulu
        lesX[0]++;          // Ici, comme la direction est DROITE alors lesX[0] est additioné afin de changer sa position, dans ce cas ci, vers la droite 
    } else if(direction==GAUCHE) {
        lesX[0]--;          // A l'inverse, lorsque la direction est GAUCHE, lesX[0] est diminué afin de changer l'oirentation du serpent
    } else if(direction==HAUT) {
        lesY[0]--;
    } else if(direction==BAS) {
        lesY[0]++;
    }
    if(plato[lesX[0]-1][lesY[0]-1]==BORDURE) {
        *contact = true;
        printf("CONTACT BORDURE");
    } else {
        for(int i = TAILLE-1 ; i >= 1; i--) {
            if (lesX[0] == lesX[i] && lesY[0] == lesY[i]) {
                *contact = true;
                printf("CONTACT SERPENT");
            }
        }
    }
    if(plato[lesX[0]-1][lesY[0]-1]==POMME) {
        *contactPomme = true;
        plato[lesX[0]-1][lesY[0]-1] = VIDE;
    }

    if(lesY[0] == 0 && plato[0][lesX[0] == VIDE]) { // Trou haut 
        lesY[0] = LARGEUR-1;
    } else if(lesY[0]-1 == LARGEUR && plato[LONGUEUR - 1][lesX[0] == VIDE]) { // Trou bas
        lesY[0] = 1;
    } else if(lesX[0]== 0 && plato[lesY[0]][0 == VIDE])  { // Trou gauche
        lesX[0] = LONGUEUR-1;
    } else if(lesX[0] == LONGUEUR && plato[lesY[0]][LONGUEUR-1 == VIDE]) { // Trou droit
        lesX[0] = 0;
    }
}

void disableEcho() { // Fonction fournis qui a pour utilité de cacher les caractères nouvellement entré dans le terminal
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Desactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

void enableEcho() { // Fonction fournis qui a pour utilité d'afficher les caractères nouvellement entré dans le terminal
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Reactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

int kbhit(){ // Fonction fournis qui a pour utilité d'e détecter si un nouveau caractère a été entré dans le terminal
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present
	
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}

void initPlateau(t_plateau plato) {         // Initialise le plateau 
    for (int i = 0; i < LONGUEUR; i++) {    // Génère la bordure du haut sur une longueur de LONGUEUR caracères
        plato[i][0] = BORDURE;
    }

    for (int i = 0; i < LONGUEUR; i++) {    // Génère la bordure du bas sur une longueur de LONGUEUR caracères
        plato[i][LARGEUR - 1] = BORDURE;
    }

    for (int j = 1; j < LARGEUR - 1; j++) {         // Génère les bordures vertical de largeur LARGEUR 
        plato[0][j] = BORDURE;                      // Un caractère BORDRURE est posé suivis de LARGEUR - 1 espace ( vide ) suivi d'une autre bordure
        for (int i = 1; i < LONGUEUR - 1; i++) {    // Ceci pour LARGEUR fois 
            plato[i][j] = VIDE;
        }
        plato[LONGUEUR - 1][j] = BORDURE;
    }

    for(int n = 0 ; n<NBBLOCS ; n++) {                      // Génère 4 blocs de hauteur 5 et de largeur 5 de BORDURE aléatoirement sur le plateau
        int randomNBX = rand()%LONGUEUR+1;                  // et les inscrémente dans le tableau a double dimension plateau
        while((randomNBX < 6 || randomNBX > LONGUEUR-8)) {
            randomNBX = rand()%LONGUEUR+1;
        }
        int randomNBY = rand()%LARGEUR+1;
        while((randomNBY < 6 || randomNBY > LARGEUR-7)) {
            randomNBY = rand()%LARGEUR+1;
        }
        for (int x = 0; x < TAILLEPAVE; x++) {
            for (int j = 0; j < TAILLEPAVE; j++) {
                plato[randomNBX + j][randomNBY] = BORDURE;
                if (randomNBX + j < LONGUEUR && randomNBY < LARGEUR) {
                    plato[randomNBX + j][randomNBY] = BORDURE;
                }
            }
            randomNBY++;
        }
    }
    plato[0][LARGEUR/2]=VIDE;                   // Génération des trous
    plato[LONGUEUR/2][0]=VIDE;
    plato[LONGUEUR-1][LARGEUR/2]=VIDE;
    plato[LONGUEUR/2][LARGEUR-1]=VIDE;
}

void dessinerPlateau(t_plateau plato) {         // Dessine le plateau Les lignes du plateau sont déssinés une à une un caractère à la fois
    for (int j = 0; j < LARGEUR; j++) {         
        for (int i = 0; i < LONGUEUR; i++) {
            printf("%c", plato[i][j]);
        }
        printf("\n"); 
    }
}

void ajouterPomme(t_plateau plato) {       // Génère deux chiffres aléatoires qui seront les positions X et Y de la pomme générée.             
    int NBX = rand()%LONGUEUR;             // Ces chiffres sont générés aléatoirement entre 0 et 80 pour les X et 0 et 40 pour les Y
    int NBY = rand()%LARGEUR;              
    while(plato[NBX][NBY]==BORDURE) {      // Un test est effectué ici afin de déterminer si ces coordonnées générées pointe sur un caractère #, si oui de nouvelles coorodonnées sont générées
        NBX = rand()%LONGUEUR;
        NBY = rand()%LARGEUR;
    }
    plato[NBX][NBY] = POMME;
}