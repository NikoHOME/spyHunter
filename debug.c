#include "main.h"

void debugChar(char *output)
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

void debugCharConsole(char *output)
{
    printf("%s\n",output);
}

void debugIntConsole(int output)
{
    printf("%d\n",output);
}