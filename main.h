#include "include/SDL_main.h"
#include "include/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CAR_HEIGHT 50
#define CAR_WIDTH 30
#define START_POSX ((SCREEN_WIDTH-CAR_WIDTH)/2)
#define START_POSY (SCREEN_HEIGHT-CAR_HEIGHT*2)
#define BORDER 160
#define FRAME_RATE 60
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define TEXT_SIZE 8

#define PAUSE_KEY SDL_SCANCODE_P
#define NEW_GAME_KEY SDL_SCANCODE_N
#define END_GAME_KEY SDL_SCANCODE_F
#define QUIT_KEY SDL_SCANCODE_ESCAPE
#define SAVE_KEY SDL_SCANCODE_S
#define LOAD_KEY SDL_SCANCODE_L
#define SHOOT_KEY SDL_SCANCODE_SPACE

#define SCORE_DISTANCE 750
#define SCORE_PER_DISTANCE 15
#define CAR_BASE_SPEED 5
#define QUIT 1
#define TRUE 1
#define FALSE 0

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

#define BLOCK_WALL 1
#define KILL_WALL 2
#define KILL_SPEED 120
#define MAX_ENEMY_START 2
#define MAX_ENEMY 5
#define ENEMY_COOLDOWN_START 2
#define CIVILIAN_SPEED 45

#define ENEMY_CHANCE 30
#define ENEMY_ARMOUR_CHANCE 30

#define SHOOT_COOLDOWN 15;

struct object
{
    SDL_Rect pos;
    int lastPosX, lastPosY;
};

struct player
{
    struct object object;
    char canShoot;
    int shootCooldown;
};

struct enemy
{
    struct object object;
    char isCivilian;
    char isArmoured;
    char dead;
    int speed;
};

struct wall
{
    char type;
    struct object object;
};

struct game
{
    struct player player;
    struct wall* wall;
    struct enemy* enemy;
    int enemySpawnMinX, enemySpawnMaxX, enemyMax;
    int enemyAmmount, enemyCooldown, enemyNext;
    int wallAmmount;
    int velocity, distance, score;
    int frameRate;
    int time,ticks;
    char move[5];
    char quit, newGame, endGame, dead, isPaused, isFrozen;
    //const Uint8 *keystate;
};

struct gameGFX
{
    SDL_Event event;
    SDL_Window *window;
	SDL_Renderer *renderer;
    SDL_Surface *charset;
    SDL_Texture *scrtex;
    
    int colors[8];
};

void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);

void run(struct game *game, struct gameGFX *gfx);
void input(struct game *game, struct gameGFX *gfx);

void movement(struct game *game);
void collision(struct game *game);
void respawn(struct game *game);
void update(struct game *game);
void spawnEnemy(struct game *game);

void drawPlayer(struct game game, struct gameGFX *gfx);
void drawRoad(struct game game, struct gameGFX *gfx);
void drawEnemy(struct game game, struct gameGFX *gfx);
void drawPause(struct game game, struct gameGFX *gfx);
void drawEndScreen(struct game game, struct gameGFX *gfx);
void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset,float scaleX, float scaleY);

void initializeGame(struct game *game);
void initializeGfx(struct gameGFX *gfx);
void initializeObj(struct object *obj, int posX, int posY, int height, int width);
void quit(struct game *game, struct gameGFX *gfx);
char loadBmp(struct gameGFX *gfx, const char *file);

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction);
char checkCollision(struct object obj1, struct object obj2);
void playerCollison(struct game *game, char direction, int index);
int framelimit(int currentTime, int lastTime, int frameRate);

void debugChar(char *output);
void debugInt(int output);
void debugCharConsole(char *output);
void debugIntConsole(int output);