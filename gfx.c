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
    for(i=0;i<game.enemyAmmount;++i)
    {
        SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);
        SDL_RenderFillRect(gfx->renderer,&game.enemy[i].object.pos);
        SDL_SetRenderDrawColor(gfx->renderer,0,0,255,255);
        SDL_RenderDrawRect(gfx->renderer,&game.enemy[i].object.pos);
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
    static char bufferVelocity[33],bufferScore[33];
    sprintf(bufferVelocity, "%d", game.velocity);
    sprintf(bufferScore, "%d", game.score);
    drawString(gfx->renderer, 20, 10, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20, 20, bufferVelocity, gfx->scrtex, 2, 2);
    SDL_RenderPresent(gfx->renderer);

}

void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset,float scaleX, float scaleY)
{
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