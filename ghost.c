#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "main.h"

void *Ghosts (void *params){

	extern char map[MAP_HEIGHT][MAP_WIDTH];
	int numdir = 0;
	int color = (rand() % (7 - 4 + 1)) + 4;	
	GhostParams * parameters = (GhostParams*) params;  
	pthread_mutex_t * mutex = parameters->state->mutex;
	ProjParams pjParamsGh[4];

	for (int i=0; i<4; i++){
		pjParamsGh[i].state = parameters->state;
	}

	/* Hides "dead" ghosts */
	while(!parameters->state->posGhost[parameters->id].alive){
		if(!parameters->state->gameover){
			parameters->state->posGhost[parameters->id].alive = 1;
		}
	}
	
	pthread_mutex_lock(mutex);
	attron(COLOR_PAIR(color));
	attron(A_BLINK);
	mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, 'M');
	attroff(A_BLINK);
	attroff(COLOR_PAIR(color));
	pthread_mutex_unlock(mutex);

	/* Wait random (1-3) seconds before moving */
	useconds_t sleepTime = (useconds_t)(((rand() % 3) + 1) * 1000000); 
	usleep(sleepTime);	

	while(parameters->state->posGhost[parameters->id].alive && parameters->state->gameover){ 
		pthread_mutex_lock(mutex);
		mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, ' ');
		pthread_mutex_unlock(mutex);
		
		numdir = getDirNum(parameters, map);
					
		CheckGhostsCollisions(parameters);

		if (isNextFree(parameters->state->posGhost, parameters->id) && numdir == 2){
			parameters->state->posGhost[parameters->id].dir.chosenDir=parameters->state->posGhost[parameters->id].dir.chosenDir;
		} else {
			parameters->state->posGhost[parameters->id].dir.chosenDir=ChoseDir(parameters, map).chosenDir;
		}
		
		switch (parameters->state->posGhost[parameters->id].dir.chosenDir){
			case DIR_UP: {
				parameters->state->posGhost[parameters->id].y-=1;
				break;
			}
			case DIR_DW: {
				if(!((parameters->state->posGhost[parameters->id].y == 11 && parameters->state->posGhost[parameters->id].x == 13) || 
					(parameters->state->posGhost[parameters->id].y == 11 && parameters->state->posGhost[parameters->id].x == 14)))
				parameters->state->posGhost[parameters->id].y+=1;
				break;
			}
			case DIR_LT: {
				if(parameters->state->posGhost[parameters->id].x == 0  && parameters->state->posGhost[parameters->id].y == 14){
					parameters->state->posGhost[parameters->id].x = 27;
					parameters->state->posGhost[parameters->id].y = 14;
				break;
			}

				parameters->state->posGhost[parameters->id].x-=1;
				break;
			}
			case DIR_RT: {
				if(parameters->state->posGhost[parameters->id].x == 27  && parameters->state->posGhost[parameters->id].y == 14){
					parameters->state->posGhost[parameters->id].x = 0;
					parameters->state->posGhost[parameters->id].y = 14;
				break;
			}
				parameters->state->posGhost[parameters->id].x+=1;
				break;
			}
		}

		if((rand() % (20 - 0 + 1)) + 0 == 5/*(time((time_t*)NULL)-timePj)>5*/){
				if(!parameters->state->posProjGh[parameters->id][DIR_UP].alive &&
				 !parameters->state->posProjGh[parameters->id][DIR_DW].alive &&
				 !parameters->state->posProjGh[parameters->id][DIR_LT].alive &&
				 !parameters->state->posProjGh[parameters->id][DIR_RT].alive){

				    for(int i=0; i<4; i++){
						if(parameters->state->thProjGh[parameters->id][i] != 0){
				    		pthread_join(parameters->state->thProjGh[parameters->id][i], NULL);
						}
						parameters->state->thProjGh[parameters->id][i] = 0;
				   		pjParamsGh[i].id = i;
						pjParamsGh[i].idGh = parameters->id;
				   		
						if(isNextFree(parameters->state->posGhost, parameters->id)){
							//parameters->state->posProjGh[parameters->id][i].alive = 1;
							switch(i){
								case DIR_UP:
									if(map[parameters->state->posGhost[parameters->id].y-1][parameters->state->posGhost[parameters->id].x] == ' '){
										parameters->state->posProjGh[parameters->id][i].alive = 1; 
										parameters->state->posProjGh[parameters->id][DIR_UP].x = parameters->state->posGhost[parameters->id].x;
										parameters->state->posProjGh[parameters->id][DIR_UP].y = parameters->state->posGhost[parameters->id].y-1;
										parameters->state->posProjGh[parameters->id][DIR_UP].dir.chosenDir = DIR_UP;
									}
									break;
								case DIR_DW:
									if(map[parameters->state->posGhost[parameters->id].y+1][parameters->state->posGhost[parameters->id].x] == ' '){
										parameters->state->posProjGh[parameters->id][i].alive = 1; 
										parameters->state->posProjGh[parameters->id][DIR_DW].x = parameters->state->posGhost[parameters->id].x;
										parameters->state->posProjGh[parameters->id][DIR_DW].y = parameters->state->posGhost[parameters->id].y+1;
										parameters->state->posProjGh[parameters->id][DIR_DW].dir.chosenDir = DIR_DW;
									}
									break;
								case DIR_LT:
									if(map[parameters->state->posGhost[parameters->id].y][parameters->state->posGhost[parameters->id].x-1] == ' '){
										parameters->state->posProjGh[parameters->id][i].alive = 1; 
										parameters->state->posProjGh[parameters->id][DIR_LT].x = parameters->state->posGhost[parameters->id].x-1;
										parameters->state->posProjGh[parameters->id][DIR_LT].y = parameters->state->posGhost[parameters->id].y;
										parameters->state->posProjGh[parameters->id][DIR_LT].dir.chosenDir = DIR_LT;
									}
									break;
								case DIR_RT:
									if(map[parameters->state->posGhost[parameters->id].y][parameters->state->posGhost[parameters->id].x+1] == ' '){
										parameters->state->posProjGh[parameters->id][i].alive = 1; 
										parameters->state->posProjGh[parameters->id][DIR_RT].x = parameters->state->posGhost[parameters->id].x+1;
										parameters->state->posProjGh[parameters->id][DIR_RT].y = parameters->state->posGhost[parameters->id].y;
										parameters->state->posProjGh[parameters->id][DIR_RT].dir.chosenDir = DIR_RT;
									}
									break;
							}	
								
							if(pthread_create(&parameters->state->thProjGh[parameters->id][i], NULL, &GhostProjectile, (void*)&pjParamsGh[i])){
								endwin();								
							}
						}
					}
			  //timePj=time((time_t*)NULL);
			}	
		}
		
	parameters->state->posGhost[parameters->id].collided = 0;
	
	pthread_mutex_lock(mutex);
	attron(COLOR_PAIR(color));
	mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, 'M');
	attroff(COLOR_PAIR(color));
	refresh();
	pthread_mutex_unlock(mutex); 
	usleep(DELAY);

	}

	for(int i=0; i<4; i++){
		if (parameters->state->thProjGh[parameters->id][i]){
			pthread_join(parameters->state->thProjGh[parameters->id][i], NULL);
			parameters->state->thProjGh[parameters->id][i] = 0;
		}
	}
	
}

void *MoveGhostIn (void *params){

	GhostParams* parameters = (GhostParams*) params;
	pthread_mutex_t * mutex = parameters->state->mutex;
	int id = parameters->id;

	pthread_mutex_lock(mutex);
	mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, ' ');
	pthread_mutex_unlock(mutex);

	parameters->state->posGhost[id].x = (rand() % (16 - 11 + 1)) + 11;
	parameters->state->posGhost[id].y = (rand() % (15 - 13 + 1)) + 13;
	
	
	pthread_join(parameters->state->thGhost[id], NULL);

	pthread_mutex_lock(mutex);
	mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, 'M');
	pthread_mutex_unlock(mutex);
	
	/* Dead time */
	usleep(3000000);
	
	pthread_mutex_lock(mutex);
	mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, ' ');
	pthread_mutex_unlock(mutex);

	if(pthread_create(&parameters->state->thmvGhostOut[parameters->id], NULL, &MoveGhostOut, (void*)parameters)){
		endwin();
	}	

	pthread_join(parameters->state->thmvGhostOut[parameters->id], NULL);
}

void *MoveGhostOut (void *params){
	GhostParams* parameters = (GhostParams*) params;
	pthread_mutex_t * mutex = parameters->state->mutex;
	int id = parameters->id;

	pthread_mutex_lock(mutex);
	mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, ' ');
	pthread_mutex_unlock(mutex);

	parameters->state->posGhost[id].x = (rand() % (14 - 13 + 1)) + 13;
	parameters->state->posGhost[id].y = 14;
	
	do {
		pthread_mutex_lock(mutex);
		mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, ' ');
		pthread_mutex_unlock(mutex);

		parameters->state->posGhost[parameters->id].y--;

		pthread_mutex_lock(mutex);
		mvaddch(parameters->state->posGhost[parameters->id].y, parameters->state->posGhost[parameters->id].x, 'M');
		//usleep(DELAY);
		//refresh();
		pthread_mutex_unlock(mutex);
	} while(parameters->state->posGhost[parameters->id].y != 11);

	parameters->state->posGhost[parameters->id].dir.chosenDir = rand()%2+2;
	parameters->state->posGhost[parameters->id].alive = 1;
	
		if(pthread_create(&parameters->state->thGhost[parameters->id], NULL, &Ghosts, (void*)parameters)){
			endwin();
		}
}

void *GhostProjectile (void *params){

	ProjParams* parameters = (ProjParams*) params;
	pthread_mutex_t * mutex = parameters->state->mutex;
	extern char map[MAP_HEIGHT][MAP_WIDTH];
	int id = parameters->idGh;
	int c = parameters->id;
	
	while(parameters->state->posProjGh[id][c].alive){
		pthread_mutex_lock(mutex);
		mvaddch(parameters->state->posProjGh[id][c].y, parameters->state->posProjGh[id][c].x,' ');
		pthread_mutex_unlock(mutex);

		switch(c){
			case DIR_UP: {
				if(map[parameters->state->posProjGh[id][c].y-1][parameters->state->posProjGh[id][c].x] == ' ' ){
					parameters->state->posProjGh[id][c].y--;
				} else {
					parameters->state->posProjGh[id][c].alive=0;
				}
				
				break;
			}
			case DIR_DW: {
				if(map[parameters->state->posProjGh[id][c].y+1][parameters->state->posProjGh[id][c].x] == ' ' && 
				!((parameters->state->posProjGh[id][c].y == 12 && parameters->state->posProjGh[id][c].x == 13) || 
					(parameters->state->posProjGh[id][c].y == 12 && parameters->state->posProjGh[id][c].x == 14))){
					parameters->state->posProjGh[id][c].y++;
				} else {
					parameters->state->posProjGh[id][c].alive=0;
				}
				break;
			}
			case DIR_LT: {
				if(parameters->state->posProjGh[id][c].x == 0  && parameters->state->posProjGh[id][c].y == 14){
					parameters->state->posProjGh[id][c].x = 27;
					parameters->state->posProjGh[id][c].y = 14;
					break;
				}
				if(map[parameters->state->posProjGh[id][c].y][parameters->state->posProjGh[id][c].x-1] == ' ' ){
					parameters->state->posProjGh[id][c].x--;
				} else {
					parameters->state->posProjGh[id][c].alive=0;
				}
				break;
			}
			case DIR_RT: {
				if(parameters->state->posProjGh[id][c].x == 27  && parameters->state->posProjGh[id][c].y == 14){
						parameters->state->posProjGh[id][c].x = 0;
						parameters->state->posProjGh[id][c].y = 14;
						break;
					}
				if(map[parameters->state->posProjGh[id][c].y][parameters->state->posProjGh[id][c].x+1] == ' ' ){
					parameters->state->posProjGh[id][c].x++;
				} else {
					parameters->state->posProjGh[id][c].alive=0;
				}
				break;
			}
		}
		pthread_mutex_lock(mutex);
		if((c == DIR_UP || c == DIR_DW) && (map[parameters->state->posProjGh[id][c].y][parameters->state->posProjGh[id][c].x] == ' ')){
			mvaddch(parameters->state->posProjGh[id][c].y, parameters->state->posProjGh[id][c].x,'|'/*ACS_VLINE*/);
		} else if((c == DIR_LT || c == DIR_RT) && (map[parameters->state->posProjGh[id][c].y][parameters->state->posProjGh[id][c].x] == ' ')) {
			mvaddch(parameters->state->posProjGh[id][c].y, parameters->state->posProjGh[id][c].x, ACS_HLINE);
		}
		refresh();
		pthread_mutex_unlock(mutex);
		usleep(DELAY/2);
	}

	pthread_mutex_lock(mutex);
	mvaddch(parameters->state->posProjGh[id][c].y, parameters->state->posProjGh[id][c].x, ' ');
	pthread_mutex_unlock(mutex);
	return NULL;
}

Dirc ChoseDir(GhostParams* g, char m[][MAP_WIDTH]){

	extern char map[MAP_HEIGHT][MAP_WIDTH];
	int possDir[4] = {0};

	g->state->posGhost[g->id].dir.nDir = 0;

	if(map[g->state->posGhost[g->id].y-1][g->state->posGhost[g->id].x] == ' '){
		if (g->state->posGhost[g->id].dir.chosenDir != DIR_DW){
			possDir[g->state->posGhost[g->id].dir.nDir++] = DIR_UP;
		}
	}
	if(map[g->state->posGhost[g->id].y+1][g->state->posGhost[g->id].x] == ' ' && !((g->state->posGhost[g->id].y == 11 && g->state->posGhost[g->id].x == 13) || 
		(g->state->posGhost[g->id].y == 11 && g->state->posGhost[g->id].x == 14))){
		if (g->state->posGhost[g->id].dir.chosenDir != DIR_UP){
			possDir[g->state->posGhost[g->id].dir.nDir++] = DIR_DW;
		}
	}
	if(((g->state->posGhost[g->id].x == 0  && g->state->posGhost[g->id].y == 14)) ||
		map[g->state->posGhost[g->id].y][g->state->posGhost[g->id].x-1] == ' '){
		if (g->state->posGhost[g->id].dir.chosenDir != DIR_RT){
			possDir[g->state->posGhost[g->id].dir.nDir++] = DIR_LT;
		}
	}
	if((g->state->posGhost[g->id].x == 27  && g->state->posGhost[g->id].y == 14) ||
		map[g->state->posGhost[g->id].y][g->state->posGhost[g->id].x+1] == ' '){
		if (g->state->posGhost[g->id].dir.chosenDir != DIR_LT){
			possDir[g->state->posGhost[g->id].dir.nDir++] = DIR_RT;
		}
	}
	
	g->state->posGhost[g->id].dir.chosenDir = possDir[rand() % g->state->posGhost[g->id].dir.nDir];
	
	return g->state->posGhost[g->id].dir;
}

bool isNextFree(Position p[], int id){

	extern char map[MAP_HEIGHT][MAP_WIDTH];	
	bool out;
	
	switch(p[id].dir.chosenDir){
		case DIR_UP: {
			if(map[p[id].y-1][p[id].x] == ' '){
				out = true;
			}
			break;
		}
		case DIR_DW: {
			if(map[p[id].y+1][p[id].x] == ' ' && ((p[id].y == 11 && p[id].x == 13) || (p[id].y == 11 && p[id].x == 14))){
				out = true;
			}
			break;
		}
		case DIR_LT: {
			if(map[p[id].y][p[id].x-1] == ' ' ){
				out = true;
			}
			break;
		}
		case DIR_RT: {
			if(MAP_WIDTH-1 && map[p[id].y][p[id].x+1] == ' '){
				out = true;
			}
			break;
		} 
	}

	return out;
}

int getDirNum (GhostParams* g, char m[][MAP_WIDTH]){
	
	extern char map[MAP_HEIGHT][MAP_WIDTH];
	int res=0;

	if(map[g->state->posGhost[g->id].y-1][g->state->posGhost[g->id].x] == ' '){
		res++;
	}
	if(map[g->state->posGhost[g->id].y+1][g->state->posGhost[g->id].x] == ' ' && !((g->state->posGhost[g->id].y == 11 && g->state->posGhost[g->id].x == 13) || 
		(g->state->posGhost[g->id].y == 11 && g->state->posGhost[g->id].x == 14))){
		res++;
	}
	if(map[g->state->posGhost[g->id].y][g->state->posGhost[g->id].x-1] == ' '){
		res++;
	}
	if(map[g->state->posGhost[g->id].y][g->state->posGhost[g->id].x+1] == ' '){
		res++;	
	}
	
	return res;
}

void invertDir (Position p[], int id){
	switch(p[id].dir.chosenDir){
		case DIR_UP:
			
			p[id].dir.chosenDir = DIR_DW;
			break;
			
		case DIR_DW:
			
			p[id].dir.chosenDir = DIR_UP;
			break;

		case DIR_LT:
			
			p[id].dir.chosenDir = DIR_RT;
			break;
			
		case DIR_RT:
			
			p[id].dir.chosenDir = DIR_LT;
			break;
		}
	return;
}

void CheckGhostsCollisions(GhostParams* p){
	switch(p->state->posGhost[p->id].dir.chosenDir){
		case DIR_UP:
			for(int i=0; i<p->state->nGhost; i++){
				if(p->state->posGhost[p->id].x == p->state->posGhost[i].x &&
					p->state->posGhost[p->id].y-1 == p->state->posGhost[i].y && 
					!((p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 13) || 
					  (p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 14))){
						invertDir(p->state->posGhost, p->id);
						invertDir(p->state->posGhost, i);
				}
			}
			break;
		
		case DIR_DW:
			for(int i=0; i<p->state->nGhost; i++){
				if(p->state->posGhost[p->id].x == p->state->posGhost[i].x &&
					p->state->posGhost[p->id].y+1 == p->state->posGhost[i].y && 
					!((p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 13) || 
					  (p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 14))){
						invertDir(p->state->posGhost, p->id);
						invertDir(p->state->posGhost, i);
				}
			}
			break;
		
		case DIR_LT:
			for(int i=0; i<p->state->nGhost; i++){
				if(p->state->posGhost[p->id].x-1 == p->state->posGhost[i].x &&
					p->state->posGhost[p->id].y == p->state->posGhost[i].y && 
					!((p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 13) || 
					  (p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 14))){
						invertDir(p->state->posGhost, p->id);
						invertDir(p->state->posGhost, i);
				}
			}
			break;
		
		case DIR_RT:
			for(int i=0; i<p->state->nGhost; i++){
				if(p->state->posGhost[p->id].x+1 == p->state->posGhost[i].x &&
					p->state->posGhost[p->id].y == p->state->posGhost[i].y && 
					!((p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 13) || 
					  (p->state->posGhost[p->id].y == 11 && p->state->posGhost[p->id].x == 14))){
						invertDir(p->state->posGhost, p->id);
						invertDir(p->state->posGhost, i);
				}
			}
			break;
	}
}
