#include "include/SDL_main.h"
#include "include/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CAR_HEIGHT 50
#define CAR_WIDTH 30
#define START_POSX ((SCREEN_WIDTH-CAR_WIDTH)/2)
#define START_POSY (SCREEN_HEIGHT-CAR_HEIGHT*2)
#define BORDER 160
#define BASE_FRAME_RATE 60
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define TEXT_SIZE 8
#define MAX_FILENAME_SIZE 32

#define PAUSE_KEY SDL_SCANCODE_P
#define NEW_GAME_KEY SDL_SCANCODE_N
#define END_GAME_KEY SDL_SCANCODE_F
#define QUIT_KEY SDL_SCANCODE_ESCAPE
#define SAVE_KEY SDL_SCANCODE_S
#define LOAD_KEY SDL_SCANCODE_L
#define SHOOT_KEY SDL_SCANCODE_SPACE
#define CONFIM_KEY SDL_SCANCODE_RETURN

#define SCORE_DISTANCE 750
#define SCORE_PER_DISTANCE 15
#define CAR_BASE_SPEED 5
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
#define STARTING_LIVES 3
#define MAX_TIME 180

#define ENEMY_CHANCE 30
#define ENEMY_ARMOUR_CHANCE 30

#define SHOOT_COOLDOWN 15
#define MAX_BULLETS 15
#define BULLET_BASE_SPEED 20
#define PLAYER_BULLET 1
#define ENEMY_BULLET 2
#define BULLET_HEIGHT 15
#define BULLET_WIDTH 15

#define KILLED_CIVILIAN_PENALTY 10
#define SCORE_PER_CAR 2000

#define WIDTH_CHANGE_CHANCE 50
#define WIDTH_CHANGE_OFFSET 25
#define WIDTH_MIN (SCREEN_WIDTH - BORDER*2)
#define WIDTH_MAX (SCREEN_WIDTH - BORDER)
#define WIDTH_MIN_DIRECTION (BORDER/2)

#define GAME_STATE_NUMBER 13
#define QUIT 0
#define NEW_GAME 1
#define END_GAME 2
#define DEAD 3
#define IS_PAUSED 4
#define IS_FROZEN 5
#define IS_SHOOTING 6
#define KILLED_CIVILIAN 7 
#define MOVE_UP 8
#define MOVE_DOWN 9
#define MOVE_RIGHT 10
#define MOVE_LEFT 11
#define TRANSITION 12

#define GAME_INTS_NUMBER 19
#define FRAME_RATE 0
#define BULLET_AMMOUNT 1
#define ENEMY_AMMOUNT 2
#define ENEMY_SPAWN_MIN_X 3
#define ENEMY_SPAWN_MAX_X 4
#define ENEMY_AMMOUNT_MAX 5
#define ENEMY_NEXT_COOLDOWN 6
#define ENEMY_NEXT 7
#define WALL_AMMOUNT 8
#define TIME 9
#define TICKS 10
#define VELOCITY 11
#define DISTANCE 12
#define SCORE 13
#define PENALTY_END 14
#define PLAYER_LIVES 15
#define LAST_DEATH 16
#define LAST_DEATH_SCORE 17
#define TRANSITION_Y 18

struct object
{
    SDL_Rect pos;
    int lastPosX, lastPosY;
};

struct player
{
    struct object object;
    int bulletCooldown, bulletNext;
};

struct enemy
{
    struct object object;
    char isCivilian;
    char isArmoured;
    char dead;
    int speed;
};

struct bullet
{
    struct object object;
    char dead, direction, owner;
    int speed;
    int distance, maxDistance;
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
    struct bullet* bullet;
    char gameState[GAME_STATE_NUMBER];
    int gameInts[GAME_INTS_NUMBER];
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

void inputCompute(struct game *game);
void collision(struct game *game);
void respawn(struct game *game);
void update(struct game *game);
void changeRoad(struct game *game);
void spawnEnemy(struct game *game);
void spawnBullet(struct game *game, int ownerIndex, char owner, char direction);
void save(struct game *game, const char name[]);
void load(struct game *game, struct wall *wall, struct enemy *enemy, struct bullet *bullet, const char name[]);
void saveInput(struct game *game, struct gameGFX *gfx);
void loadInput(struct game *game, struct gameGFX *gfx);

void drawPlayer(struct game game, struct gameGFX *gfx);
void drawRoad(struct game game, struct gameGFX *gfx);
void drawEnemy(struct game game, struct gameGFX *gfx);
void drawBullet(struct game game, struct gameGFX *gfx);
void drawPause(struct game game, struct gameGFX *gfx);
void drawEndScreen(struct game game, struct gameGFX *gfx);
void drawSave(struct gameGFX *gfx, const char input[]);
void drawLoad(struct gameGFX *gfx, char *input[], int select, int fileNumber);
void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset,float scaleX, float scaleY);
void drawWhiteText(struct gameGFX *gfx, int x, int y, float scaleX, float scaleY, const char input[]);

void initializeGame(struct game *game);
void initializeGfx(struct gameGFX *gfx);
void initializeObj(struct object *obj, int posX, int posY, int height, int width);
void quit(struct game *game, struct gameGFX *gfx);
char loadBmp(struct gameGFX *gfx, const char *file);

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction);
char checkCollision(struct object obj1, struct object obj2);
void playerCollision(struct game *game, char direction, int index);
void enemyCollision(struct game *game, char direction, int index, int index2);
int framelimit(int currentTime, int lastTime, int frameRate);

void debugChar(const char *output);
void debugInt(int output);
void debugCharConsole(const char *output);
void debugIntConsole(int output);