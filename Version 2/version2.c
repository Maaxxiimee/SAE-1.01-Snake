#include <fcntl.h> // Definition des librairies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define TAILLE 11 // Taille du serpent
#define DELAI 90000 // Délai de 90 millisecondes

int tabX[TAILLE], tabY[TAILLE]; // Déclaration des variables
const char TETE = 'O'; // Tete du serpent
const char CORP = 'X'; // Corp du serpent
const char ARRET = 'a'; // Touche d'arret du programme
const char HAUT = 'z'; // Déclaration des variables concernant le controle du serpent
const char BAS = 's';
const char GAUCHE = 'q';
const char DROITE = 'd';
const int POS_INIT_X = 20;
const int POS_INIT_Y = 20;

void gotoXY(int x, int y);
void afficher(int x, int y, char c);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction);
void disableEcho();
void enableEcho();
int kbhit();
char kbhitChar();


/***************************************************
 *                                                 *
 *          Début du programme principal (main)    *
 *                                                 *
 ***************************************************/

int main() {
	char touche = DROITE; // Direction par défaut du serpent
    char nouvelleTouche; // Stockage temporaire de la touche entrée

	// Initialisation des deux tableaux contenant les coordonées du serpent
    tabX[0] = POS_INIT_X;
    tabY[0] = POS_INIT_Y;
    for (int i = 1; i < TAILLE; i++) {
        tabX[i] = POS_INIT_X - i;
        tabY[i] = POS_INIT_Y;
    }

    system("clear"); // Nettoyage du contenu du terminal
    disableEcho();  
    printf("\033[?25l"); // Enlève le curseur afin d'éviter un bug d'affichage lorsque le serepent disparait

    while (touche!=ARRET) { // Démarrage de la boucle while / du mouvement du serpent
        disableEcho(); // désactive l'affichage dans le terminal des caractères entrés
        dessinerSerpent(tabX, tabY);
        usleep(DELAI); // Delai de 90ms entre chaque mise à jour de l'avancée du serpent
        progresser(tabX, tabY, touche);   
        if(kbhit()) { // Test des conditions afin de détecter si la toiche pressé est connue et si elle n'est pas l'opposé de la touche actuelle
            scanf("%c", &nouvelleTouche);
            if ((nouvelleTouche == HAUT || nouvelleTouche == BAS || nouvelleTouche == GAUCHE || nouvelleTouche == DROITE || nouvelleTouche == ARRET) && 
            !(touche==DROITE && nouvelleTouche==GAUCHE) && !(touche==GAUCHE && nouvelleTouche==DROITE) && !(touche==HAUT && nouvelleTouche==BAS) && !(touche==BAS && nouvelleTouche==HAUT)) {
                touche = nouvelleTouche;
            }
        }           
    }

    printf("\033[?25h"); // Ré-active le curseur
    enableEcho();
    return 0;
}


void gotoXY(int x, int y) { 
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

void dessinerSerpent(int lesX[], int lesY[]) { // Dessine le serpent. Prend maintenant en compte la position de la tête afin de l'afficher ou non
    int i;                             

    for (i = 0; i < TAILLE; i++) {
        effacer(lesX[i], lesY[i]);
    }

    for (i = 1; i < TAILLE; i++) {
        if (lesX[i] >= 0 && lesY[i] >= 0) {
            afficher(lesX[i], lesY[i], CORP);
        }
    }

    if (lesX[0] >= 0 && lesY[0] >= 0) {
        afficher(lesX[0], lesY[0], TETE);
    }
}


void progresser(int lesX[], int lesY[], char direction) {
    effacer(lesX[TAILLE-1], lesY[TAILLE-1]); // Positionne tous les éléments du serpent à une position x+1 en effacant le dernier élément afin d'avoir seulement 9 anneaux
    for(int i = TAILLE - 1; i > 0; i--) {
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