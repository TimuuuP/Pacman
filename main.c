#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "main.h"

char map[MAP_HEIGHT][MAP_WIDTH] ={
	"+--------------------------+",
	"|            ||            |",
	"| |--| |---| || |---| |--| |",
	"| |--| |---| || |---| |--| |",
	"| |--| |---| [] |---| |--| |",
	"|                          |",
	"| |--| [] [--[]--| [] |--| |",
	"| |--| || [--||--] || |--| |",
	"|      ||    ||    ||      |",
	"|----] ||--| || |--|| [----|",
	"|----| ||--] [] |--|| |----|",
	"|----| ||          || |----|",
	"|----| || []-  -[] || |----|",
	"|----] [] |      | [] [----|",
	"          |      |          ",
	"|----] [] |      | [] [----|",
	"|----| || [------] || |----|",
	"|----| ||          || |----|",
	"|----| || [------] || |----|",
	"|----] [] [------] [] [----|",
	"|            ||            |",
	"| [--] [---] || [---] [--] |",
	"| [--| [---] [] [---] |--] |",
	"|   ||                ||   |",
	"|-] || [] [--[]--] [] || [-|",
	"|-] [] || [--||--] || [] [-|",
	"|      ||    ||    ||      |",
	"| [----][--] || [--][----] |",
	"| [--------] [] [--------] |",
	"|                          |",
	"+--------------------------+",
};

char dotsMap[MAP_HEIGHT][MAP_WIDTH] ={
	"+--------------------------+",
	"|  .   .   . || .   .   .  |",
	"| |--| |---|.||.|---| |--| |",
	"|.|--| |---| || |---|.|--|.|",
	"| |--| |---|.[].|---| |--| |",
	"|.    .   .   .  .   .    .|",
	"| |--| [] [--[]--| [] |--| |",
	"|.|--|.|| [--||--] ||.|--|.|",
	"| . .  ||.   ||   .|| . .  |",
	"|----].||--| || |--||.[----|",
	"|----| ||--] [] |--|| |----|",
	"|----|.||  .    .  ||.|----|",
	"|----| ||.[--  --].|| |----|",
	"|----].[] |      | [].[----|",
	"  . .    .|      |.    . .  ",
	"|----].[] |      | [].[----|",
	"|----| ||.[------].|| |----|",
	"|----|.||  . . .   ||.|----|",
	"|----| ||.[------] || |----|",
	"|----] [] [------] [].[----|",
	"|  .  .  .  .||.  .  .  .  |",
	"|.[--] [---] || [---] [--].|",
	"| [--| [---].[].[---] |--] |",
	"|.  ||.   .   .   .  .||  .|",
	"|-].|| [] [--[]--] [] ||.[-|",
	"|-] [] ||.[--||--].|| [] [-|",
	"|  .  .|| .  ||  . ||.  .  |",
	"|.[----][--] || [--][----].|",
	"| [--------] [] [--------] |",
	"| . . . . . .  . . . . . . |",
	"+--------------------------+",
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]){

	int row, col;
	GameState gamestate = {0};
	gamestate.gameover = 1;
	gamestate.score = 0;
	gamestate.pacLives = 3;
	gamestate.nGhost = MIN_GHOST_ONSCREEN;

	if(argc > 3){
		printf("\nTOO MANY ARGUMENTS!\n");
		printf("\nUsage: first input is number of lives\nsecond input is number of ghosts.\n\n");
		exit(0);
	} else if (atoi(argv[2]) > MAX_GHOST){
		printf("\nTOO MANY GHOSTS!\n");
		printf("\nPlease choose a number between %d and %d.\n\n", MIN_GHOST_ONSCREEN, MAX_GHOST);
		exit(0);
	} else if (argc == 3){
		gamestate.pacLives = atoi(argv[1]);
		gamestate.nGhost = atoi(argv[2]);
	}

	GhostParams ghParams[gamestate.nGhost];

	memset(ghParams, 0, sizeof(ghParams));
	
	
	//GhostParams *ghParams = calloc (gamestate.nGhost, sizeof(GhostParams));

	for(int i=0; i<gamestate.nGhost; i++){
		ghParams[i].id = i;
		ghParams[i].state = &gamestate;
	}

	gamestate.posGhost[0].x = 26;
	gamestate.posGhost[0].y = 1;
	gamestate.posGhost[1].x = 26;
	gamestate.posGhost[1].y = 29;
	gamestate.posGhost[2].x = 1;
	gamestate.posGhost[2].y = 29;
	
	for(int i=MIN_GHOST_ONSCREEN; i<gamestate.nGhost; i++){
		if(!gamestate.posGhost[i].alive){
			gamestate.posGhost[i].x = 14;
			gamestate.posGhost[i].y = 14;
		}
	}
	
	memcpy(gamestate.map, dotsMap, MAP_HEIGHT*MAP_WIDTH*sizeof(char));


	gamestate.mutex = &mutex;
		
	/*nCurses initialization*/	    
	initscr();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	cbreak();
	curs_set(0);
	start_color();
	init_pair(MAP_PAIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(PAC_PAIR, COLOR_YELLOW, COLOR_BLACK);
	init_pair(DOT_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(GHOST_PAIR1, COLOR_RED, COLOR_BLACK);
	init_pair(GHOST_PAIR2, COLOR_GREEN, COLOR_BLACK);
	init_pair(GHOST_PAIR3, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(GHOST_PAIR4, COLOR_CYAN, COLOR_BLACK);

	srand(time(NULL));

	//mvprintw(MAP_HEIGHT+4,0, "Arguments: %d %d", gamestate.pacLives, gamestate.nGhost);
	
	if(pthread_create(&gamestate.thLevel, NULL, &Level, (void*)&ghParams)) {
		endwin();
	}

	if(pthread_create(&gamestate.thPacman, NULL, &Pacman, (void*)&gamestate)) {
		endwin();
	}
	
	for(int i=0; i<gamestate.nGhost; i++) {
		gamestate.posGhost[i].alive = (i < MIN_GHOST_ONSCREEN);
	   if(pthread_create(&gamestate.thGhost[i], NULL, &Ghosts, (void*)&ghParams[i])) {
			endwin();
		}
	}

	

	pthread_join (gamestate.thLevel, NULL);
	pthread_join (gamestate.thPacman, NULL);
	
	for(int i=0; i<gamestate.nGhost; i++){
		pthread_join(gamestate.thGhost[i], NULL);
	}

	pthread_mutex_destroy (&mutex);
	
	/* ENDGAME */
	clear();
	getmaxyx(stdscr,row,col);
	wborder(stdscr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, '+', '+', '+', '+');
 	mvprintw((row/2)-1, col/2 -7, "GAMEOVER!");
 	
	if(gamestate.pacLives == 0)
		mvprintw(row/2, col/2 -7, "YOU LOST!");
	else
		mvprintw(row/2, col/2 -7, "YOU WON!");
		
	mvprintw((row/2)+1, col/2 -15, "LIVES LEFT: %d | SCORE: %3d", gamestate.pacLives, gamestate.score);
	mvprintw((row/2)+3, col/2 -12, "PRESS ENTER TO EXIT.");
	
	while (getch()!='\n');

	endwin();	

	return 0;
}


void *Level (void *params){

	int aliveGhosts = MIN_GHOST_ONSCREEN;
	int hits=0;
	time_t lifeTime = time((time_t*)NULL);
	
	GhostParams * parameters = (GhostParams*) params;
	pthread_mutex_t * mutex = parameters->state->mutex;
	
	do{
		
		/* Pac-Dot + Ghost awakening */
		if(dotsMap[parameters->state->posPac.y][parameters->state->posPac.x] == '.'){
			dotsMap[parameters->state->posPac.y][parameters->state->posPac.x] = ' ';
			parameters->state->score++;
			for(int i=0; i<parameters->state->nGhost; i++){
				if(!parameters->state->posGhost[i].alive && aliveGhosts < (parameters->state->nGhost)){
					aliveGhosts++;
					parameters->state->posGhost[i].alive = 1;
					parameters->state->posGhost[i].collided = 1;
					parameters->state->posGhost[i].x = parameters->state->posPac.x;
					parameters->state->posGhost[i].y = parameters->state->posPac.y;
					break;
				}
			}
		}
		
		/* Pac-Ghost */
		for(int i=0; i<parameters->state->nGhost; i++){
			if(parameters->state->posPac.x == parameters->state->posGhost[i].x &&
			   parameters->state->posPac.y == parameters->state->posGhost[i].y &&
				parameters->state->posGhost[i].collided == 0){
				parameters->state->posGhost[i].collided = 1;
				/* "Invulnerability" time */
				if((time((time_t*)NULL)-lifeTime)>1){
					parameters->state->pacLives--;
					lifeTime=time((time_t*)NULL);
				}
			}
		}
		
		/* ProjPac-Dot */
		for(int i=0; i<4; i++){
			if(dotsMap[parameters->state->posProjPac[i].y][parameters->state->posProjPac[i].x] == '.'){
				parameters->state->posProjPac[i].alive = 0;
			}
		}

		/* ProjGhost-Dot */
		for(int i=0; i<parameters->state->nGhost; i++){
			for(int j=0; j<4; j++){
				if(dotsMap[parameters->state->posProjGh[i][j].y][parameters->state->posProjGh[i][j].x] == '.'){
					parameters->state->posProjGh[i][j].alive = 0;
				}
			}
		}
				
		/* ProjGhost-Pac */
		for(int i=0; i<parameters->state->nGhost; i++){
			for(int j=0; j<4; j++){		
				if(parameters->state->posProjGh[i][j].x == parameters->state->posPac.x &&
				   parameters->state->posProjGh[i][j].y == parameters->state->posPac.y &&
				   parameters->state->posProjGh[i][j].alive == 1 && parameters->state->posGhost[i].alive == 1){

					parameters->state->posProjGh[i][j].alive = 0;
					
					hits++;
					
					if(hits == 10){
						parameters->state->pacLives--;
						hits = 0;
					}
				}
			}
		}
		
		/* ProjPac-Ghost */
		for(int i=0; i<parameters->state->nGhost; i++){
			for(int j=0; j<4; j++){
				if(parameters->state->posGhost[i].x == parameters->state->posProjPac[j].x &&
				   parameters->state->posGhost[i].y == parameters->state->posProjPac[j].y &&
				   parameters->state->posProjPac[j].alive == 1){

					parameters->state->posProjPac[j].alive = 0;
					parameters->state->posGhost[i].alive = 0;
					if(pthread_create(&parameters->state->thmvGhostIn[i], NULL, &MoveGhostIn, (void*)&parameters[i])){
							endwin();
					}
				}
			}
		}
		

	pthread_mutex_lock(mutex);
	PrintDots(dotsMap);
	mvprintw(MAP_HEIGHT, 0,   "+--------------------------+");
	//mvchgat(MAP_HEIGHT, 0, -1, A_NORMAL, MAP_PAIR, NULL);
	mvprintw(MAP_HEIGHT+1, 0, "|   LIVES: %2d | SCORE: %2d  |",  parameters->state->pacLives, parameters->state->score);
	//mvchgat(MAP_HEIGHT+1, 0, 1, A_NORMAL, MAP_PAIR, NULL);
	//mvchgat(MAP_HEIGHT+1, 14, 1, A_NORMAL, MAP_PAIR, NULL);
	//mvchgat(MAP_HEIGHT+1, 27, 1, A_NORMAL, MAP_PAIR, NULL);
	mvprintw(MAP_HEIGHT+2, 0, "+--------------------------+");
	//mvchgat(MAP_HEIGHT+2, 0, -1, A_NORMAL, MAP_PAIR, NULL);	
	refresh();
	pthread_mutex_unlock(mutex);
	

	if(parameters->state->pacLives == 0 || parameters->state->score == DOTS){
		parameters->state->gameover = 0;
	}

	} while (parameters->state->gameover);
	
	for(int i=0; i<parameters->state->nGhost; i++){
		pthread_join(parameters->state->thmvGhostIn[i], NULL);
	}
}

void PrintDots (char m[][MAP_WIDTH]){
int row,col;
  for(row=0;row<MAP_HEIGHT;row++){
	for(col=0;col<MAP_WIDTH;col++){
		if(m[row][col] == '.'){
			attron(COLOR_PAIR(DOT_PAIR));
	  		mvaddch(row,col,ACS_BULLET);
			attroff(COLOR_PAIR(DOT_PAIR));
		} else if (m[row][col] == '_') {
			attron(COLOR_PAIR(MAP_PAIR));
			mvaddch(row,col,' ');
			attroff(COLOR_PAIR(MAP_PAIR));
		} else if (m[row][col] != ' ' && m[row][col] != '.') {
			attron(COLOR_PAIR(MAP_PAIR));
			mvaddch(row,col,ACS_CKBOARD);
			attroff(COLOR_PAIR(MAP_PAIR));
		} 
	}
  }
}
