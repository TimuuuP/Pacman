#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "main.h"

void *Pacman (void *params){

	extern char map[MAP_HEIGHT][MAP_WIDTH];
	int key;
	ProjParams pjParams[4] = {};
	GameState * parameters = (GameState*) params;  
	pthread_mutex_t * mutex = parameters->mutex;

	for (int i=0; i<4; i++){
		pjParams[i].id = i;
		pjParams[i].state = parameters;
	}
	
	parameters->posPac.x = 1;	
	parameters->posPac.y = 1;

	while(parameters->gameover){
		pthread_mutex_lock(mutex);		
		key = getch();
		flushinp();
		
		
		mvaddch(parameters->posPac.y, parameters->posPac.x,' ');
		pthread_mutex_unlock(mutex);
			
		parameters->posPac.dir.nDir=0;
		
		switch(key){
			case KEY_UP:
				if(parameters->posPac.y > 0 && map[parameters->posPac.y-1][parameters->posPac.x] == ' ')
					parameters->posPac.dir.chosenDir=DIR_UP;
				break;
			case KEY_DOWN:
				if(parameters->posPac.y < MAP_HEIGHT-1 && map[parameters->posPac.y+1][parameters->posPac.x] == ' ' &&
				   !((parameters->posPac.y == 11 && parameters->posPac.x == 13) || 
				    (parameters->posPac.y == 11 && parameters->posPac.x == 14)))
					parameters->posPac.dir.chosenDir=DIR_DW;
				break;
			case KEY_LEFT:
				if(parameters->posPac.x > 0 && map[parameters->posPac.y][parameters->posPac.x-1] == ' ')
					parameters->posPac.dir.chosenDir=DIR_LT;
				break;
			case KEY_RIGHT:
				if(parameters->posPac.x < MAP_WIDTH-1 && map[parameters->posPac.y][parameters->posPac.x+1] == ' ')
					parameters->posPac.dir.chosenDir=DIR_RT;
				break;
			case KEY_PPAGE:
					parameters->pacLives = 0;
				break;
			case SPACE:
				if(!parameters->posProjPac[DIR_UP].alive &&
				   !parameters->posProjPac[DIR_DW].alive &&
				   !parameters->posProjPac[DIR_LT].alive &&
				   !parameters->posProjPac[DIR_RT].alive){
				    for(int i=0; i<4; i++){

				    	pthread_join(parameters->thProjPac[i], NULL);
						parameters->thProjPac[i] = 0;
				   		pjParams[i].id = i;
				   		
						if(isNextFreePac(parameters->posPac, i)){
							//parameters->posProjPac[i].alive = 1;
							switch(i){
								case DIR_UP:
									if(map[parameters->posPac.y-1][parameters->posPac.x]  == ' '){
										parameters->posProjPac[i].alive = 1;
										parameters->posProjPac[DIR_UP].x = parameters->posPac.x;
										parameters->posProjPac[DIR_UP].y = parameters->posPac.y-1;
										parameters->posProjPac[DIR_UP].dir.chosenDir = DIR_UP;
									}
									break;
								case DIR_DW:
									if(map[parameters->posPac.y+1][parameters->posPac.x]  == ' '){
										parameters->posProjPac[i].alive = 1;
										parameters->posProjPac[DIR_DW].x = parameters->posPac.x;
										parameters->posProjPac[DIR_DW].y = parameters->posPac.y+1;
										parameters->posProjPac[DIR_DW].dir.chosenDir = DIR_DW;
									}
									break;
								case DIR_LT:
									if(map[parameters->posPac.y][parameters->posPac.x-1]  == ' '){
										parameters->posProjPac[i].alive = 1;
										parameters->posProjPac[DIR_LT].x = parameters->posPac.x-1;
										parameters->posProjPac[DIR_LT].y = parameters->posPac.y;
										parameters->posProjPac[DIR_LT].dir.chosenDir = DIR_LT;
									}
									break;
								case DIR_RT:
									if(map[parameters->posPac.y][parameters->posPac.x+1]  == ' '){
										parameters->posProjPac[i].alive = 1;
										parameters->posProjPac[DIR_RT].x = parameters->posPac.x+1;
										parameters->posProjPac[DIR_RT].y = parameters->posPac.y;
										parameters->posProjPac[DIR_RT].dir.chosenDir = DIR_RT;
									}
									break;
							}	
								
							if(pthread_create(&parameters->thProjPac[i], NULL, &Projectile, (void*)&pjParams[i])){
								endwin();								
							}
						}
					}
				}
				break;
		}
		
		switch(parameters->posPac.dir.chosenDir){
			case DIR_UP:
				if(parameters->posPac.y > 0 && map[parameters->posPac.y-1][parameters->posPac.x] == ' ')
					parameters->posPac.y-=1;
				break;
			case DIR_DW:
				if(parameters->posPac.y < MAP_HEIGHT-1 && map[parameters->posPac.y+1][parameters->posPac.x] == ' ' && !((parameters->posPac.y == 11 && parameters->posPac.x == 13) || 
					(parameters->posPac.y == 11 && parameters->posPac.x == 14)))
					parameters->posPac.y+=1;
				break;
			case DIR_LT:
				if(parameters->posPac.x == 0  && parameters->posPac.y == 14){
					parameters->posPac.x = 27;
					parameters->posPac.y = 14;
					break;
				}
				
				if(parameters->posPac.y > 0 && map[parameters->posPac.y][parameters->posPac.x-1] == ' ')
					parameters->posPac.x-=1;
				break;
			case DIR_RT:
				if(parameters->posPac.x == 27  && parameters->posPac.y == 14){
					parameters->posPac.x = 0;
					parameters->posPac.y = 14;
					break;
				}
				if(parameters->posPac.y > 0 && map[parameters->posPac.y][parameters->posPac.x+1] == ' ')
					parameters->posPac.x+=1;
				break;
		}

		pthread_mutex_lock(mutex);
		attron(COLOR_PAIR(PAC_PAIR));		
		switch(parameters->posPac.dir.chosenDir){
			case DIR_UP:
				mvaddch(parameters->posPac.y,parameters->posPac.x,'v');
				break;
			case DIR_DW:
				mvaddch(parameters->posPac.y,parameters->posPac.x,ACS_UARROW);
				break;
			case DIR_LT:
				mvaddch(parameters->posPac.y,parameters->posPac.x,ACS_RARROW);
				break;
			case DIR_RT:
				mvaddch(parameters->posPac.y,parameters->posPac.x,ACS_LARROW);
				break;
		}
		attroff(COLOR_PAIR(PAC_PAIR));
		
		pthread_mutex_unlock(mutex);
		usleep(DELAY);
	}
	
	for(int i=0; i<4; i++){
		if (parameters->thProjPac[i])
			pthread_join(parameters->thProjPac[i], NULL);
	}
}

void *Projectile (void *params){

	ProjParams* parameters = (ProjParams*) params;
	pthread_mutex_t * mutex = parameters->state->mutex;
	int c = parameters->id;
	extern char map[MAP_HEIGHT][MAP_WIDTH];

	pthread_mutex_lock(mutex);
	if((c == DIR_UP || c == DIR_DW) /*&& (map[parameters->state->posProjPac[c].y][parameters->state->posProjPac[c].x] == ' ')*/){
		mvaddch(parameters->state->posProjPac[c].y, parameters->state->posProjPac[c].x,'|');
	} else if((c == DIR_LT || c == DIR_RT) /*&& (map[parameters->state->posProjPac[c].y][parameters->state->posProjPac[c].x] == ' ')*/) {
		mvaddch(parameters->state->posProjPac[c].y, parameters->state->posProjPac[c].x, ACS_HLINE);
	}
	pthread_mutex_unlock(mutex);
	
	while(parameters->state->posProjPac[c].alive){

			pthread_mutex_lock(mutex);
			mvaddch(parameters->state->posProjPac[c].y,parameters->state->posProjPac[c].x,' ');
			pthread_mutex_unlock(mutex);

		switch(c){
			case DIR_UP: {
				if(map[parameters->state->posProjPac[DIR_UP].y][parameters->state->posProjPac[DIR_UP].x] == ' ' ){
					parameters->state->posProjPac[DIR_UP].y--;
				} else {
					parameters->state->posProjPac[c].alive=0;
				}

				break;
			}
			case DIR_DW: {
				if(map[parameters->state->posProjPac[DIR_DW].y][parameters->state->posProjPac[DIR_DW].x] == ' ' && 
				!((parameters->state->posProjPac[DIR_DW].y == 12 && parameters->state->posProjPac[DIR_DW].x == 13) || 
					(parameters->state->posProjPac[DIR_DW].y == 12 && parameters->state->posProjPac[DIR_DW].x == 14))){
					parameters->state->posProjPac[DIR_DW].y++;
				} else {
					parameters->state->posProjPac[c].alive=0;
				}
				break;
			}
			case DIR_LT: {
				if(parameters->state->posProjPac[DIR_LT].x == 0  && parameters->state->posProjPac[DIR_LT].y == 14){
					parameters->state->posProjPac[DIR_LT].x = 27;
					parameters->state->posProjPac[DIR_LT].y = 14;
					break;
				}
				if(map[parameters->state->posProjPac[DIR_LT].y][parameters->state->posProjPac[DIR_LT].x] == ' ' ){
					parameters->state->posProjPac[DIR_LT].x--;
				} else {
					parameters->state->posProjPac[c].alive=0;
				}
				break;
			}
			case DIR_RT: {
				if(parameters->state->posProjPac[DIR_RT].x == 27  && parameters->state->posProjPac[DIR_RT].y == 14){
						parameters->state->posProjPac[DIR_RT].x = 0;
						parameters->state->posProjPac[DIR_RT].y = 14;
						break;
					}
				if(map[parameters->state->posProjPac[DIR_RT].y][parameters->state->posProjPac[DIR_RT].x] == ' ' ){
					parameters->state->posProjPac[DIR_RT].x++;
				} else {
					parameters->state->posProjPac[c].alive=0;
				}
				break;
			}
		}
			
		pthread_mutex_lock(mutex);
		if((c == DIR_UP || c == DIR_DW) && (map[parameters->state->posProjPac[c].y][parameters->state->posProjPac[c].x] == ' ')){
			mvaddch(parameters->state->posProjPac[c].y, parameters->state->posProjPac[c].x,'|');
		} else if((c == DIR_LT || c == DIR_RT) && (map[parameters->state->posProjPac[c].y][parameters->state->posProjPac[c].x] == ' ')) {
			mvaddch(parameters->state->posProjPac[c].y, parameters->state->posProjPac[c].x, ACS_HLINE);
		}
		refresh();
		pthread_mutex_unlock(mutex);
		usleep(DELAY/2);
		
	}
	
	pthread_mutex_lock(mutex);
	mvaddch(parameters->state->posProjPac[c].y, parameters->state->posProjPac[c].x, ' ');
	pthread_mutex_unlock(mutex);
	return NULL;
}

bool isNextFreePac(Position p, int id){

	extern char map[MAP_HEIGHT][MAP_WIDTH];	

	switch(id){
		case DIR_UP: {
				if(map[p.y-1][p.x] == ' ')
					return true;
				break;
			}
			case DIR_DW: {
				if(map[p.y+1][p.x] == ' ' && !((p.y == 11 && p.x == 13) || (p.y != 11 && p.x == 14)))
					return true;
				break;
			}
			case DIR_LT: {
				if(map[p.y][p.x-1] == ' ' )
					return true;
				break;
			}
			case DIR_RT: {
				if(map[p.y][p.x+1] == ' ')
					return true;
				break;
			} 
	}

	return false;
}
