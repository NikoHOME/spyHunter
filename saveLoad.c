#include "main.h"

void save(struct game *game, const char name[])
{
    FILE *file = fopen(name, "wb");

    fwrite(&game->player, sizeof(struct player), 1, file);
    fwrite(&game->gameState, sizeof(game->gameState), 1, file);
    fwrite(&game->gameInts, sizeof(game->gameInts), 1, file);

    fwrite(game->wall, sizeof(struct wall), WALL_AMMOUNT, file);
    fwrite(game->enemy, sizeof(struct enemy), game->gameInts[ENEMY_AMMOUNT], file);
    fwrite(game->bullet, sizeof(struct bullet), MAX_BULLETS, file);
    fclose(file);
}

void load(struct game *game, struct wall *wall, struct enemy *enemy, struct bullet *bullet, const char name[])
{
    
    FILE *file = fopen(name, "rb");

    fread(&game->player, sizeof(struct player), 1, file);
    fread(&game->gameState, sizeof(game->gameState), 1, file);
    fread(&game->gameInts, sizeof(game->gameInts), 1, file);
    
    //free(wall);
    //wall = (struct wall*)(malloc(sizeof(struct wall) * length));
    fread(wall, sizeof(struct wall), WALL_AMMOUNT, file);
    //free(enemy);
    //enemy = (struct enemy *)(malloc(sizeof(struct enemy) * length));
    fread(enemy, sizeof(struct enemy), game->gameInts[ENEMY_AMMOUNT], file);

    //free(bullet);
    //bullet = (struct bullet *)(malloc(sizeof(struct bullet) * length));
    fread(bullet, sizeof(struct bullet), MAX_BULLETS, file);

    fclose(file);
}



void saveInput(struct game *game, struct gameGFX *gfx)
{
    
    FILE *file;
    char input[MAX_FILENAME_SIZE+1], inputf[MAX_FILENAME_SIZE+1];
    input[0]=' ';
    input[1]='\0';
    int nameSize = 0, index;
    char exitWrite = FALSE;
    drawSave(gfx, input);
    const char textSave[] = "Save already exists";
    int currentTime, lastTime = SDL_GetTicks();
    while(!exitWrite)
    {
        currentTime = SDL_GetTicks();
        usleep(framelimit(currentTime, lastTime, game->gameInts[FRAME_RATE]));
        lastTime = currentTime;
        SDL_PollEvent(&gfx->event);
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[QUIT_KEY])
                {
                    exitWrite = 2;
                }
                if(keystate[CONFIM_KEY])
                {
                    snprintf(inputf, sizeof(inputf), "save/%s.save", input);
                    if(file = fopen(inputf, "r"))
                    {
                        drawWhiteText(gfx, SCREEN_WIDTH/2-strlen(textSave)*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE*3, 2, 2, textSave);
                    }
                    else
                        exitWrite = TRUE;
                }
                for(index = SDL_SCANCODE_1; index <= SDL_SCANCODE_0; ++index)
                {
                    if(keystate[index])
                    {
                        if(index == SDL_SCANCODE_0)
                            input[nameSize]='0';
                        else
                            input[nameSize]='0'+index-SDL_SCANCODE_1+1;
                        ++nameSize;
                        input[nameSize]='\0';
                        drawSave(gfx,input);
                    }
                }
                for(index = SDL_SCANCODE_A; index <= SDL_SCANCODE_Z; ++index)
                {
                    if(keystate[index])
                    {
                        input[nameSize]='A'+index-SDL_SCANCODE_A;
                        ++nameSize;
                        input[nameSize]='\0';
                        drawSave(gfx,input);
                    }
                }
                if(keystate[SDL_SCANCODE_BACKSPACE])
                {
                    if(nameSize>0)
                    {
                        nameSize--;
                        input[nameSize]='\0';
                        drawSave(gfx,input);
                    }
                }
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }
    if(exitWrite == TRUE)
        save(game, inputf);
    game->gameState[IS_PAUSED] = TRUE;
    drawPause(*game,gfx);
}

void loadInput(struct game *game, struct gameGFX *gfx)
{
    FILE *file;
    DIR *dir = opendir("save");
    struct dirent *read;
    int fileNumber = 0, index = 0;
    while((read = readdir(dir)) != NULL) 
    {
        if ( !strcmp(read->d_name, ".") || !strcmp(read->d_name, "..") )
            continue;
        ++fileNumber;
    }
    rewinddir(dir);
    char *filesList[fileNumber];
    while((read = readdir(dir)) != NULL) {
        if ( !strcmp(read->d_name, ".") || !strcmp(read->d_name, "..") )
            continue;
        filesList[index] = (char*) malloc (strlen(read->d_name)+1);
        strncpy (filesList[index],read->d_name, strlen(read->d_name) );
        filesList[index] = read->d_name;
        ++index;
    }
    rewinddir(dir);

    char inputf[MAX_FILENAME_SIZE+1];
    drawLoad(gfx, filesList, 0, fileNumber);
    char exitWrite = FALSE;
    int select = 0;
    int currentTime, lastTime = SDL_GetTicks();
    while(!exitWrite)
    {
        currentTime = SDL_GetTicks();
        usleep(framelimit(currentTime, lastTime, game->gameInts[FRAME_RATE]));
        lastTime = currentTime;
        SDL_PollEvent(&gfx->event);
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[QUIT_KEY])
                {
                    exitWrite = 2;
                }
                if(keystate[CONFIM_KEY])
                {
                    exitWrite = TRUE;
                }
                if(keystate[SDL_SCANCODE_UP])
                {
                    if(select > 0)
                        --select;
                    drawLoad(gfx, filesList, select, fileNumber);
                }
                if(keystate[SDL_SCANCODE_DOWN])
                {
                    if(select < fileNumber)
                        ++select;
                    drawLoad(gfx, filesList, select, fileNumber);
                }   
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }
    snprintf(inputf, sizeof(inputf), "save/%s", filesList[select]);
    load(game, game->wall, game->enemy, game->bullet, inputf);
}