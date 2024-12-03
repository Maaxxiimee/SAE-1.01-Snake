#include <fcntl.h> // Definition des librairies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define TAILLE 10 // Taille du serpent
#define VALMIN 0 // Valeur minimale du tableau
#define VALMAX 40 // Valeur maximale du tableau
#define DELAI 500000 // Délai de 500 millisecondes

int tabX[TAILLE], tabY[TAILLE]; // Déclaration des variables
const char TETE = 'O'; // Tete du serpent
const char CORP = 'X'; // Corp du serpent

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

void dessinerSerpent(int lesX[], int lesY[]) { // Dessine le serpent dans le terminal en affichant la tête du serpent en premier et ensuite ses éléments
	int i;
	afficher(lesX[0], lesY[0], TETE); 
	for(i = 1; i < TAILLE; i++) {
		afficher(lesX[i], lesY[i], CORP); 
	}
}

void progresser(int lesX[], int lesY[]) { // Positionne tous les éléments du serpent à une posotion x+1 en effacant le dernier élément afin d'avoir seulement 9 anneaux
	effacer(lesX[TAILLE-1], lesY[TAILLE-1]);
	for(int i = TAILLE - 1; i > 0; i--) {
		lesX[i] = lesX[i-1];
		lesY[i] = lesY[i-1];
	}
	lesX[0]++;
}

int kbhit() {
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere present
	int unCaractere = 0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// Mettre le terminal en mode non-bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	// Restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF) {
		ungetc(ch, stdin);
		if (getchar() == 'a') { // getchar() afin d'analyser l'entrée d'un caractère dans le terminal. Arrêt si le caractère est "a"
                unCaractere = 1;          
        }
	} 
	return unCaractere;
}

int main() { // Démarrage du programme

    int posInitX, posInitY; // Définition des variables posInitX et posInitY qui seront les conteneurs de valeurs entrées par l'utilisateur

	// Test de posInitX : Doit être compris entre 1 et 40
    printf("Valeur de la position initiale du serpent en abscisse ( X ) : "); 
    scanf("%d", &posInitX);
    while (posInitX < VALMIN || posInitX > VALMAX || posInitX == VALMIN) {
        printf("Saisie invalide, la valeur saisie doit être comprise entre 1 et 40\nValeur de la position initiale du serpent en abscisse ( X ) : ");
        scanf("%d", &posInitX);
    }

	// Test de posInitX : Doit être compris entre 1 et 40
    printf("Valeur de la position initiale du serpent en ordonnée ( Y ) : ");
    scanf("%d", &posInitY);
    while (posInitY < VALMIN || posInitY > VALMAX || posInitY == VALMIN) {
        printf("Saisie invalide, la valeur saisie doit être comprise entre 1 et 40\nValeur de la position initiale du serpent en ordonnée ( Y ) : ");
        scanf("%d", &posInitY);
    }

	// Initialisation des deux tableaux contenant les coordonées du serpent
    tabX[0] = posInitX;
    tabY[0] = posInitY;
    for (int i = 1; i < TAILLE; i++) {
        tabX[i] = posInitX - i;
        tabY[i] = posInitY;
    }

    system("clear"); // Nettoyage du contenu du terminal

    while (kbhit()==0) { // Démarrage de la boucle while / du mouvement du serpent
        dessinerSerpent(tabX, tabY);  
        usleep(DELAI); // Delai de 500ms entre chaque mise à jour de l'avanc&ée du serpent
        progresser(tabX, tabY);              
    }

    return 0;
}
