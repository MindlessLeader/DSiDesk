#ifndef SCREEN_H
#define SCREEN_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>
#include <stdlib.h>

typedef struct
{
    Display *display;
    XImage *image;
    Window root;
    int screenWidth;
    int screenHeight;
    int xOffset;
    int yOffset;
} ScreenStruct;

ScreenStruct *initScreen(int screenWidth, int screenHeight, int xOffset, int yOffset);
char* getScreenData(ScreenStruct* screenStruct);
void closeScreen(ScreenStruct* screenStruct);

#endif