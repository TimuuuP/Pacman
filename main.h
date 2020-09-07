/* HEADER FILE */

/* GAME VARIABLES */
#define MIN_GHOST_ONSCREEN 3
#define MAX_GHOST 8
#define DELAY 100000

/* CONSTANTS */
#define MAP_HEIGHT 31
#define MAP_WIDTH 28
#define MAXX 100
#define MAXY 100
#define SPACE ' '
#define DIR_UP 0
#define DIR_DW 1
#define DIR_LT 2
#define DIR_RT 3
#define MAP_PAIR 1
#define PAC_PAIR 2
#define DOT_PAIR 3
#define GHOST_PAIR1 4
#define GHOST_PAIR2 5
#define GHOST_PAIR3 6
#define GHOST_PAIR4 7
#define DOTS 100

/* STRUCTURES */
typedef struct{
	int chosenDir;
	int nDir;
} Dirc;

typedef struct{
	Dirc dir;
	int x; 
	int y; 
	int alive;
	int collided;
} Position;

typedef struct {
	int score;
    int pacLives;
	int nGhost;
	int gameover;
	Position posPac;
	Position posProjPac[4];
	Position posProjGh[MAX_GHOST][4];
	Position posGhost[MAX_GHOST];
	pthread_mutex_t* mutex;
	pthread_t thPacman;
	pthread_t thLevel;
	pthread_t thProjPac[4];
	pthread_t thGhost[MAX_GHOST];
	pthread_t thmvGhostIn[MAX_GHOST];
	pthread_t thmvGhostOut[MAX_GHOST];
	pthread_t thProjGh[MAX_GHOST][4];	
	char map[MAP_HEIGHT][MAP_WIDTH];
} GameState;

typedef struct {
    int id;
    GameState* state;
} GhostParams;

typedef struct {
    int id;
	int idGh;
    GameState* state;
} ProjParams;

/* THREADS */
void *Level (void *params);
void *Pacman (void *params);
void *Ghosts (void *params);
void *Projectile (void *params);
void *GhostProjectile (void *params);
void *MoveGhostIn (void *params);
void *MoveGhostOut (void *params);
/* GENERAL FUN */
void PrintDots (char m[][MAP_WIDTH]);
void invertDir (Position p[], int id);
void CheckGhostsCollisions(GhostParams* p);
int getDirNum (GhostParams* g, char m[][MAP_WIDTH]);
bool isNextFree(Position p[], int id);
bool isNextFreePac(Position p, int id);
Dirc ChoseDir (GhostParams* g, char m[][MAP_WIDTH]);
