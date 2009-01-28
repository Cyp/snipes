#include "help.h"

#include <string.h>


static int const helpWidth = 50;
static int const helpHeight = 29;
static char const helpText[50*29] =
        "|                                                |"
        "|               Welcome to Snipes.               |"
        "|                                                |"
        "|To open/close this help screen, press F3.       |"
        "|                                                |"
        "|Move around the maze, avoiding the snipe      \x10\x11|"
        "|bullets \x88, looking for the snipe generators,  \x12\x13|"
        "|shooting all the snipes \x80\x88 and snipe generators.|"
        "|                                                |"
        "|             \x17\x18\x19                                |"
        "|This is you: \x1A\x1B\x1C   \x82   \x83   \x82   \x83                |"
        "|             \x1D\x1E\x1F                                |"
        "|                                                |"
        "|Movement: Arrow keys, hold Space to go faster.  |"
        "|Shooting: Q\x8D,W\x8E,E\x8F,  A\x8C,S\x8A,D\x88,  Z\x8B,X\x8A,C\x89        |"
        "|                                                |"
        "|Pause: P or Pause/Break key                     |"
        "|                                                |"
        "|Fullscreen: Alt+Enter                           |"
        "|Change resolution: F8, Up/Down, Enter           |"
        "|Change colours:    F9, Up/Down, Enter           |"
        "|                                                |"
        "|Sound off: F1    Sound on: F2                   |"
        "|                                                |"
        "|Quit: Escape, N, Enter                          |"
        "|                                                |"
        "|                                                |"
        "|http://cyp.github.com/snipes/                   |"
        "|                                                |"
        ;

void renderHelp(unsigned char *disp, int width, int height)
{
    int y;
    for(y = 0; y < helpHeight; ++y)
        memcpy(disp + (width-helpWidth)/2 + ((height-helpHeight)/2 + y)*width, helpText + y*helpWidth, helpWidth);
}
