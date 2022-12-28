#include "main.h"

// Check if time is greater than framerate
char framelimit(int currentTime,int lastTime)
{
    if( (currentTime - lastTime) * FRAME_RATE < 1000 ) // 1000 = 1 second 
        return 1;
    return 0;
}

void debugChar(char* output)
{
    FILE *ptr = fopen("debug.txt","a");
    fprintf(ptr,"%s\n",output);
    fclose(ptr);
}
void debugInt(int output)
{
    FILE *ptr = fopen("debug.txt","a");
    fprintf(ptr,"%d\n",output);
    fclose(ptr);
}

void initializeObj(struct object *obj,int posX,int posY,int height,int width)
{
    obj->pos.x=posX;
    obj->pos.y=posX;
    obj->pos.h=height;
    obj->pos.w=width;
}

char checkCollision(struct object obj1, struct object obj2)
{
    if(MIN(obj1.pos.x+obj1.pos.w,obj2.pos.x+obj2.pos.w)<=MAX(obj1.pos.x,obj2.pos.x))
        return 0;
    if(MAX(obj1.pos.y-obj1.pos.h,obj2.pos.y-obj2.pos.h)>=MIN(obj1.pos.y,obj2.pos.y))
        return 0;
    if(obj1.lastPosY<obj2.lastPosY-obj2.pos.h)
        return DOWN;
    if(obj2.lastPosY<obj1.lastPosY-obj1.pos.h)
        return UP;
    if(obj1.lastPosX+obj1.pos.w<obj2.lastPosX)
        return RIGHT;
    return LEFT;
    
}

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction)
{
	if(keystate[key])
    {
        game->move[direction]=1;
    }
    else 
    {
        game->move[direction]=0;
    }
}


void input(struct game *game, struct gameGFX *gfx, char *quit)
{
    
    while(SDL_PollEvent(&gfx->event)) 
    {
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN: case SDL_KEYUP:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
				keyCheck(game,gfx,keystate,SDL_SCANCODE_UP,UP);
                keyCheck(game,gfx,keystate,SDL_SCANCODE_DOWN,DOWN);
                keyCheck(game,gfx,keystate,SDL_SCANCODE_LEFT,LEFT);
                keyCheck(game,gfx,keystate,SDL_SCANCODE_RIGHT,RIGHT);
                break;
			case SDL_QUIT:
                *quit=1;
				break;
		}
	}
}

void movement(struct game *game)
{
    if(game->move[UP])
        game->player.object.pos.y-=CAR_SPEED;
    if(game->move[DOWN])
        game->player.object.pos.y+=CAR_SPEED;
    if(game->move[LEFT])
        game->player.object.pos.x-=CAR_SPEED;
    if(game->move[RIGHT])
        game->player.object.pos.x+=CAR_SPEED;
}

