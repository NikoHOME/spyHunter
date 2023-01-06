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

#define CAR_BASE_SPEED 10
#define QUIT 1
#define TRUE 1
#define FALSE 0

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

#define BLOCK_WALL 1
#define KILL_WALL 2

struct object
{
    SDL_Rect pos;
    int lastPosX, lastPosY;
};

struct player
{
    struct object object;
};

struct enemy
{
    struct object object;
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
    int wallAmmount;
    int velocity;
    int frameRate;
    char move[5];
    char quit;
    char dead;
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

void drawPlayer(struct game game, struct gameGFX *gfx);
void drawRoad(struct game game, struct gameGFX *gfx);
void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset);

void initializeGame(struct game *game);
void initializeGfx(struct gameGFX *gfx);
void initializeObj(struct object *obj, int posX, int posY, int height, int width);
void quit(struct game *game, struct gameGFX *gfx);
char loadBmp(struct gameGFX *gfx, const char *file);

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction);
char checkCollision(struct object obj1, struct object obj2);
char framelimit(int currentTime, int lastTime, int frameRate);

void debugChar(char *output);
void debugInt(int output);
void debugCharConsole(char *output);
void debugIntConsole(int output);