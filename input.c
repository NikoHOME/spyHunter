#include "main.h"

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction)
{
	if(keystate[key])
    {
        game->gameState[direction] = TRUE;
    }
    else 
    {
        game->gameState[direction] = FALSE;
    }
}


void input(struct game *game, struct gameGFX *gfx)
{
    
    while(SDL_PollEvent(&gfx->event)) 
    {
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN: case SDL_KEYUP:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[PAUSE_KEY])
                {
                    if(game->gameState[IS_PAUSED])
                    {
                        game->gameState[IS_PAUSED] = FALSE;
                        return;
                    }
                    game->gameState[IS_PAUSED] = TRUE;
                    drawPause(*game,gfx);
                }
                if(keystate[SHOOT_KEY])
                {
                    game->gameState[IS_SHOOTING] = TRUE;
                }
                else
                    game->gameState[IS_SHOOTING] = FALSE;
                if(keystate[QUIT_KEY])
                    game->gameState[QUIT] = TRUE;
                if(keystate[NEW_GAME_KEY])
                {
                    game->gameState[QUIT] = TRUE;
                    game->gameState[NEW_GAME]= TRUE;
                }
                if(keystate[END_GAME_KEY] && game->gameState[END_GAME] == FALSE)
                {
                    game->gameState[QUIT] = TRUE;
                    game->gameState[END_GAME] = TRUE;
                }
                if(keystate[SAVE_KEY])
                {
                    saveInput(game, gfx);
                }
                if(keystate[LOAD_KEY])
                {
                    loadInput(game, gfx);
                }
				keyCheck(game, gfx, keystate, SDL_SCANCODE_UP, MOVE_UP);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_DOWN, MOVE_DOWN);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_LEFT, MOVE_LEFT);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_RIGHT, MOVE_RIGHT);
                break;
			case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
		}
	}
}

void inputCompute(struct game *game)
{
    if(game->gameState[IS_FROZEN])
        return;
    if(game->gameState[IS_SHOOTING])
        spawnBullet(game, FALSE, PLAYER_BULLET, FALSE);

    game->player.object.lastPosX = game->player.object.pos.x;
    game->player.object.lastPosY = game->player.object.pos.y;

    if(game->gameState[MOVE_UP] && game->player.object.pos.y >= SCREEN_HEIGHT/2)
    {
        game->player.object.pos.y -= CAR_BASE_SPEED;
        game->gameInts[VELOCITY] += CAR_BASE_SPEED;
    }
    if(game->player.object.pos.y >= START_POSY)
        return;

    if(game->gameState[MOVE_DOWN])
    {
        game->player.object.pos.y += CAR_BASE_SPEED/2;
        game->gameInts[VELOCITY] -= CAR_BASE_SPEED/2;
    }
    if(game->gameState[MOVE_LEFT])
        game->player.object.pos.x -= CAR_BASE_SPEED;

    if(game->gameState[MOVE_RIGHT])
        game->player.object.pos.x += CAR_BASE_SPEED;
    
}