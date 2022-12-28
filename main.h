#include "include/SDL_main.h"
#include "include/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BORDER 40
#define FRAME_RATE 60
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define CAR_SPEED 10
#define QUIT 1

#define UP 1
#define DOWN 2
#define RIGHT 3
#define LEFT 4

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

struct game
{
    struct player player;
    char move[5];
    //const Uint8 *keystate;
};

struct gameGFX
{
    SDL_Event event;
    SDL_Window *window;
	SDL_Renderer *renderer;
    SDL_Surface *screen;
    
    int colors[8];
};

void DrawString(SDL_Surface *screen, int x, int y, const char *text, SDL_Surface *charset);
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);

void run(struct game *game, struct gameGFX *gfx);
void input(struct game *game, struct gameGFX *gfx, char *quit);
void draw(struct game game, struct gameGFX *gfx);
void movement(struct game *game);

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction);
char checkCollision(struct object obj1, struct object obj2);
char framelimit(int currentTime,int lastTime);
void debugChar(char* output);
void debugInt(int output);