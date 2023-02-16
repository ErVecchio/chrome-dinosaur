
/**
	\file game.c
	\author Matteo Cordaro
	\versione 1.0
	\date 25/01/2023
*/




#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "menu.h"
#include "game.h"
#include "appearance.h"
#include "minunit.h"

int test_run=0;
int test_passed = 0;

void test_computeTime() {
mu_assert("ERRORE in funzione computeTime(), (250000>=250000)",computeTime(250000)==249000);
    mu_assert("ERRORE in funzione computeTime(), (300000>=250000)",computeTime(300000)==299000);
    mu_assert("ERRORE in funzione computeTime(), (560000>=250000)",computeTime(560000)==559000);
    mu_assert("ERRORE in funzione computeTime(), (230000>=200000)",computeTime(230000)==229400);
    mu_assert("ERRORE in funzione computeTime(), (200000>=200000)",computeTime(200000)==199400);
    mu_assert("ERRORE in funzione computeTime(), (229000>=200000)",computeTime(229000)==223000);
    mu_assert("ERRORE in funzione computeTime(), (190000<200000)",computeTime(190000)==189800);
    mu_assert("ERRORE in funzione computeTime(), (66000<200000)",computeTime(66000)==65800);
    mu_assert("ERRORE in funzione computeTime(), (1000<200000)",computeTime(1000)==800);
    mu_assert("ERRORE in funzione computeTime(), (150000<200000)-200",computeTime(150000)==148000);
}

void all_test() {
	mu_run_test(test_checkGame);
	mu_run_test(test_computeTime);
	mu_run_test(test_computePrize);
}


int run_all_test(int argc, char **argv) {
	if(argv<2 || strcmp(argv[1], "-test")!=0)
	{
		return -1;
	}
	printf("\nESEGUO I TEST\n");
	all_test();
	printf("\nTest eseguiti: %d\n",test_run);
	printf("\nTest eseguiti con successo: %d\n",test_passed);
}

// Check if the game is going to be finished in this turn of loop

/**
	\fn int checkGame(int y, int x, int diY, int diX)
	\brief Controlla se il gioco finira\' alla fine del ciclo
	\param int y: prima coordinata
	\param int x: seconda coordinata
	\param int diX: distanza sul piano delle X
	\param int diY: distanza sul piano delle Y
	\return int: valore che indica la fine del gioco
*/
int checkGame(int y, int x, int diY, int diX) {
	if (diY == y) {
		if (abs((diX+14)-x) <= 4) {
			return 0;
		}
	}
	return 1;
}

// Make game faster

/**
	\fn int computeTime(int delayTime)
	\brief Rende il gioco piu\' veloce modificando il delay
	\param int delayTime: tempo di delay tra un'azione e l'altra
	\return int: tempo di delay
*/
int computeTime(int delayTime) {
	if (delayTime >= 250000) {
		delayTime -= 1000;
	}
	else if (delayTime >= 200000) {
		delayTime -= 600;
	}
	else {
		delayTime -= 200;
	}
	return delayTime;
}

// Which dinosaur should be printed

/**
	\fn void showdinosaur(int diY, int diX)
	\bief Questa procedura stampa un differente dinosauro in base a determinati parametri
	\param int diX: distanza sul piano delle X
	\param int diY: distanza sul piano delle Y
	\return int: mostrera\' il primo o il secondo dinosauro
*/
void showdinosaur(int diY, int diX) {
	static int counter = 0;
	if (counter == 0) {
		dinosaur1(diY, diX);
		counter++;
	}
	else {
		dinosaur2(diY, diX);
		counter--;
	}
}

// Give user the prize

/**
	\fn int computePrize(int score, int usedPrize)
	\brief Questa funzione dara\' il punteggio effettuato
	\param int score: punteggio
	\param int usedPrize:
	\return int: punteggio finale
*/
int computePrize(int score, int usedPrize) {
	if (score >= 20 && score <= 40 && usedPrize == 0) {
		return 1;
	}
	else if (score >= 60 && score <= 80 && usedPrize <= 1) {
		return 1;
	}
	else if (score >= 100 && score <= 120 && usedPrize <= 2) {
		return 1;
	}	
	return 0;
}

// The main engine!

/**
	\fn void startEngine(int highScore, struct user firstUser)
	\brief Questa procedura compone il codice principale del gioco: e\' presente il funzionamento del gioco, il punteggio, i vari colori presenti e l'alternarsi del giorno e della notte.
	\param int highScore: punteggio massimo
	\return int: punteggio ottenuto a fine gioco, distanza percorsa, e verra\' visualizzato un detrminato colore
*/
void startEngine(int highScore, struct user firstUser) {
    srand(time(NULL));
	int x, y, diX=5, prize=0, usedPrize=0, score=0, delayTime = 300000
    , gameStatus=1, cactusNum=0;
	int maxX=getmaxx(stdscr);
	x = maxX-20;
	y = getmaxy(stdscr)-6;
	int diY = y;
	int arrowX=(diX+15), arrowY=(diY-3);
	char userInput;
	int jumping=-1;
    bool fire=FALSE;
    clear();
	nodelay(stdscr, TRUE);
  	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(4,COLOR_BLUE,COLOR_BLACK);
	init_pair(5,COLOR_GREEN,COLOR_BLACK);
	init_pair(6,COLOR_YELLOW,COLOR_BLACK);
	while (gameStatus == 1) {
		userInput = getch();
		// Show day or night
		if((score/50)%2 != 0) {
			moon(10, (maxX/2)-10);
		}
		else {
			attron(COLOR_PAIR(1));
			sun(10, (maxX/2)-10);			
		}
		// clear arrow
		if (fire) {
			mvprintw(arrowY, arrowX-2, " ");
		}

		score++;
        // Show informations
		mvprintw(1, 6, "%s %s %s", firstUser.name, firstUser.lastName, firstUser.age);
		mvprintw(1, getmaxx(stdscr)-9, "%d:%d", highScore, score);
		// Use prize to destroy cactus
		prize = computePrize(score, usedPrize);
		mvprintw(3, 6, "Prize: %d    ", prize);
		if (prize == 1) {
			if (userInput == 'k') {
				usedPrize++;
				fire = TRUE;
			}
		}
		if (fire) {
			mvprintw(arrowY, arrowX, "*");
			arrowX += 2;
		}
		if ((x+4)-arrowX <= 1 && fire) {
			clearCactus1(y, x-1);
			mvprintw(arrowY, arrowX-2, " ");
			x = getmaxx(stdscr)-20;
			fire = FALSE;
			arrowX = diX+15;
		}
		// ----------
		box(stdscr, ACS_VLINE, ACS_HLINE);
		//for clearing screen
		cleardinosaurUp(diY, diX);
		if (x <= 7) {
			x = getmaxx(stdscr)-20;
            cactusNum = rand() % 2;
		}
        // if input is equal to ' ' then jump
		if (userInput == ' ' && jumping<0) {
			diY -= 7;
			jumping = 3;
		}
		showdinosaur(diY, diX);
		if (userInput == ' ') {
			cleardinosaurDown(diY, diX);		
		}
		if (x-diX <= 7) {
			x -= 10;
		}
        if (cactusNum == 0) {
        	cactus1(y, x);
		}
		else {
			cactus2(y, x);
		}
		if (x-diX <= 7) {
			x += 10;
		}
		gameStatus = checkGame(y, x, diY, diX);
        // Bring back dinosaur
		jumping--;
		if (jumping==0) {
			diY += 7;
		}
		mvhline(y+1, 1, '-', getmaxx(stdscr)-3);
        refresh();
        clearCactus1(y, x);
		refresh();
        usleep(delayTime);
        x -= 7;
        delayTime = computeTime(delayTime);
		userInput = 'q';
	}

	endGame(score, highScore, diY, diX, firstUser);
	attroff(COLOR_PAIR(1));
}


//
// START of tests section
//

// sample variables to be tested
int foo = 4;
int bar = 5;

// init tests vars
int tests_run = 0;
int tests_passed = 0;

// sample test function 1
/**
	\fn static char * test_foo()
	\brief Una semplice funzione che indica degli errori nei test prima dell'inizio del gioco
*/
static char * test_foo() {
    mu_assert("error, foo != 7", foo == 7);
}

// sample test function 2
/**
	\fn static char * test_bar()
	\brief Una semplice funzione che indica degli errori nei test prima dell'inizio del gioco
*/
static char * test_bar() {
    mu_assert("error, bar != 5", bar == 5);
}

// put all tests here
/**
	\fn void all_tests()
	\brief Una procedura che inserisce i test qui
*/
void all_tests() {
	mu_run_test(test_foo);
	mu_run_test(test_bar);
}

// call this to run all tests
/**
	\fn int run_all_tests(int argc, char **argv)
	\brief Una funziona che chiama le due funzioni precedenti
	\param int argc: un parametro che controlla i test
	\return int: Inidichera\' se i test sono stati passati e quanti ne sono stati eseguiti
*/
int run_all_tests(int argc, char **argv) {
	if(argc<2 || strcmp(argv[1],"-test")!=0) {
		return -1;
	}
	
	printf("--- RUNNING TESTS ---\n");
    all_tests();
	printf("--- SUMMARY ---\n");
    printf("Total number of tests: %d\n", tests_run);
	printf("Tests passed: %d\n", tests_passed);
	return tests_run-tests_passed;
}

void test_checkGame
{
	int oracolo = 0;
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x=4", checkGame(5, 20, 5, 10) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(2, 17, 2, 3) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diY==y)", checkGame(5, 20, 5, 10) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(7, 140, 7, 3) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(7, 32, 7, 5) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(4, 39, 4, 1) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(4, 28, 4, 2) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(2, 17, 2, 3) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diY=y)", checkGame(7, 140, 7, 3) == oracolo);
	mu_assert("Errore nella funzione check_Game(), (diX+14)-x<4", checkGame(7, 320, 7, 5) == oracolo);
}
//
// END of tests section
//
