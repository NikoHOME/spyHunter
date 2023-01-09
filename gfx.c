#include "main.h"

void drawPlayer(struct game game, struct gameGFX *gfx)
{
    
    SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);
    SDL_RenderFillRect(gfx->renderer,&game.player.object.pos);
    SDL_SetRenderDrawColor(gfx->renderer,0,0,255,255);
    SDL_RenderDrawRect(gfx->renderer,&game.player.object.pos);
    SDL_RenderPresent(gfx->renderer);
}

void drawEnemy(struct game game, struct gameGFX *gfx)
{
    int i;
    for(i=0;i<MAX_ENEMY;++i)
    {
        if(game.enemy[i].dead)
            continue;
        if(game.enemy[i].isCivilian)
            SDL_SetRenderDrawColor(gfx->renderer,255,255,0,255);
        else
            SDL_SetRenderDrawColor(gfx->renderer,0,45,210,255);
        if(game.enemy[i].isArmoured)
            SDL_SetRenderDrawColor(gfx->renderer,115,0,210,255);
        SDL_RenderFillRect(gfx->renderer,&game.enemy[i].object.pos);

    }
    SDL_RenderPresent(gfx->renderer);
}

void drawRoad(struct game game, struct gameGFX *gfx)
{
    SDL_SetRenderDrawColor( gfx->renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect( gfx->renderer, NULL );

    SDL_SetRenderDrawColor(gfx->renderer,0,255,0,255);
    SDL_Rect leftSide,rightSide;
    leftSide.x=0;
    rightSide.x=SCREEN_WIDTH-BORDER;
    leftSide.y=rightSide.y=0;
    leftSide.h=rightSide.h=SCREEN_HEIGHT;
    leftSide.w=rightSide.w=BORDER;
    SDL_RenderFillRect(gfx->renderer,&leftSide);
    SDL_RenderFillRect(gfx->renderer,&rightSide);

    static char bufferVelocity[33],bufferScore[33],bufferTime[33];
    sprintf(bufferVelocity, "%d", game.velocity);
    sprintf(bufferScore, "%d", game.score);
    sprintf(bufferTime, "%d", game.time);
    
    drawString(gfx->renderer, 40, 20, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 40, 40, bufferVelocity, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 40, SCREEN_HEIGHT-40, bufferTime, gfx->scrtex, 2, 2);

    static const char menu[8][20]= 
    {
        "arrows: move",
        "esc: quit",
        "n: new game",
        "s: save",
        "l: load",
        "p: pause",
        "space: shoot",
        "f: end game"
    };
    int i=0,j=0;
    for(;i<8;++i,j+=15)
    {
        drawString(gfx->renderer, SCREEN_WIDTH-BORDER+10, SCREEN_HEIGHT/2+100+j, menu[i], gfx->scrtex, 1.5, 1.5);
    }
    SDL_RenderPresent(gfx->renderer);
}

void drawPause(struct game game, struct gameGFX *gfx)
{
    drawRoad(game,gfx);
    drawPlayer(game,gfx);
    drawEnemy(game,gfx);
    SDL_RenderPresent(gfx->renderer);
    SDL_SetRenderDrawColor(gfx->renderer,0,0,0,100);
    SDL_Rect box;

    box.x=SCREEN_WIDTH/4;
    box.y=SCREEN_HEIGHT/4;
    box.h=SCREEN_HEIGHT/2;
    box.w=SCREEN_WIDTH/2;

    static const char text[]= "PAUSE";
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer,0,0,20,100);
    SDL_RenderFillRect(gfx->renderer,&box);
    SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(text)*TEXT_SIZE, SCREEN_HEIGHT/2-TEXT_SIZE, text, gfx->scrtex, 2, 2);

    SDL_RenderPresent(gfx->renderer);
}

void drawEndScreen(struct game game, struct gameGFX *gfx)
{
    static char bufferScore[40],bufferTime[40];
    static const char text[3][10] = 
    {
        "Score: ",
        "Time: ",
        "Game Over"
    };
        static const char menu[2][20]= 
    {
        "esc: quit",
        "n: new game",
    };
    sprintf(bufferScore, "%s%d", text[0], game.score);
    sprintf(bufferTime, "%s%d", text[1], game.time); 
    SDL_SetRenderDrawColor(gfx->renderer,0,0,0,255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(text[2])*TEXT_SIZE*2, SCREEN_HEIGHT/2-TEXT_SIZE*8, text[2], gfx->scrtex, 4, 4);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(bufferScore)*TEXT_SIZE, SCREEN_HEIGHT/2-TEXT_SIZE, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(bufferTime)*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE, bufferTime, gfx->scrtex, 2, 2);

    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(menu[0])*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE*3, menu[0], gfx->scrtex, 2, 2);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(menu[1])*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE*5, menu[1], gfx->scrtex, 2, 2);

    SDL_RenderPresent(gfx->renderer);
}
void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset,float scaleX, float scaleY)
{
    x/=scaleX;
    y/=scaleY;
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8*scaleX;
	d.h = 8*scaleY;
	while(*text)
    {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x*scaleX;
		d.y = y*scaleY;
		SDL_RenderCopy(screen, charset, &s, &d);
		x += 8;
		text++;
	}
}