#include "../include/SDL_main.h"
#include "../include/SDL.h"
#include "../include/SDL_image.h"
#include <stdio.h>
#include <time.h>
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
#define BORDER 200
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
#define RESPAWN_TIME 1000
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
#define MAX_POWERUP 5
#define ENEMY_COOLDOWN_START 2
#define CIVILIAN_SPEED 45
#define STARTING_LIVES 3
#define ENEMY_CHANCE 30
#define ENEMY_ARMOUR_CHANCE 20
#define ENEMY_POWERUP_RANGE_CHANCE 30
#define ENEMY_POWERUP_SCORE_CHANCE 30
#define SCORE_PER_POWERUP 1000
#define POWERUP_RANGE_DURATION 15
#define POWERUP_RANGE 0
#define POWERUP_SCORE 1



#define SHOOT_COOLDOWN 15
#define MAX_BULLETS 15
#define BULLET_BASE_SPEED 20
#define PLAYER_BULLET 1
#define ENEMY_BULLET 2
#define BULLET_HEIGHT 16
#define BULLET_WIDTH 16

#define KILLED_CIVILIAN_PENALTY 10
#define SCORE_PER_CAR 2000
#define CIVILIAN_STUN 120
#define ENEMY_STUN 60
#define ARMORED_STUN 5
#define ATTACK_DISTANCE 25
#define ATTACK_DELAY 120
#define ATTACK_DURATION 60
#define ATTACK_DURATION_BEGIN 90

#define WIDTH_CHANGE_CHANCE 10
#define WIDTH_CHANGE_OFFSET (25)
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

#define GAME_INTS_NUMBER 21
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
#define POWERUP_DURATION 19
#define POWERUP_AMMOUNT 20

#define TEXTURE_NUMBER 7
#define TEXTURE_PLAYER 0
#define TEXTURE_ENEMY 1
#define TEXTURE_ARMOUR 2
#define TEXTURE_POWERUP 3
#define TEXTURE_BOX 4
#define TEXTURE_BULLET 5
#define TEXTURE_CIVIL 6

#define CAR_TEXTURE_SIZE 64

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
    struct object view[5];
    char isCivilian;
    char isArmoured;
    char isAttacking;
    char isPowerup;
    char isPowerupScore;
    char dead;
    int speed, stunned, attack, attackPower;
};

struct bullet
{
    struct object object;
    char dead, direction, owner;
    int speed;
    int distance, maxDistance;
};

struct box
{
    char type, dead;
    int speed;
    struct object object;
};

struct game
{
    struct player player;
    struct box* wall;
    struct box* powerup;
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
    SDL_Texture *objectTexture[TEXTURE_NUMBER];
};

struct entry
{
    int score, time;
};
struct scoreList
{
    int length;
    struct entry *entry;
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
void handleRespawn(struct game *game, struct gameGFX *gfx, int *currentTime, int *lastTime);
void update(struct game *game);
void changeRoad(struct game *game);
void spawnEnemy(struct game *game);
void spawnBox(struct game *game, int ownerIndex, char type);
void spawnBullet(struct game *game, int ownerIndex, char owner, char direction);
void save(struct game *game, const char name[]);
void load(struct game *game, const char name[]);
void saveInput(struct game *game, struct gameGFX *gfx);
void saveScore(struct game *game, struct gameGFX *gfx);
void loadInput(struct game *game, struct gameGFX *gfx);
void loadScore(struct game *game, struct gameGFX *gfx);

void drawMain(struct game game, struct gameGFX *gfx);
void drawPlayer(struct game game, struct gameGFX *gfx);
void drawRoad(struct game game, struct gameGFX *gfx);
void drawEnemy(struct game game, struct gameGFX *gfx);
void drawBullet(struct game game, struct gameGFX *gfx);
void drawPause(struct game game, struct gameGFX *gfx);
void drawEndScreen(struct game game, struct gameGFX *gfx);
void drawSave(struct gameGFX *gfx);
void drawLoad(struct gameGFX *gfx, char *input[], int select, int fileNumber);
void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset,float scaleX, float scaleY);
void drawWhiteText(struct gameGFX *gfx, int x, int y, float scaleX, float scaleY, const char input[]);
void drawScore(struct gameGFX *gfx, struct entry *entry, int select, int length);
void drawPowerup(struct game game, struct gameGFX *gfx);

void initializeGame(struct game *game);
void initializeGfx(struct gameGFX *gfx);
void initializeObj(struct object *obj, int posX, int posY, int height, int width);
void freeGame(struct game *game);
void freeGFX(struct gameGFX *gfx);
void loadBmp(struct gameGFX *gfx, const char *file);

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction);
char checkCollision(struct object obj1, struct object obj2);
void playerCollision(struct game *game, char direction, int index);
void enemyCollision(struct game *game, char direction, int index, int index2);
void bulletCollision(struct game *game, int index, int index2);
void powerupCollision(struct game *game, int index, int index2);
int framelimit(int currentTime, int lastTime, int frameRate);
int compareTime(const void *s1, const void *s2);
int compareScore(const void *s1, const void *s2);
void sleepTillNextFrame(struct game *game, int *currentTime, int *lastTime);
struct object viewToPosition(struct enemy enemy, int direction);

void debugChar(const char *output);
void debugInt(int output);
void debugCharConsole(const char *output);
void debugIntConsole(int output);