#ifndef MOUSE_H
#define MOUSE_H

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum MouseButton
{
    LEFT_CLICK = 1,
    MIDDLE_CLICK = 2,
    RIGHT_CLICK = 3
};

typedef struct
{
    Display *display;
    Window root;
    bool pressed[3];
}Mouse;

Mouse* getMouse();
void setMousePos(Mouse* mouse, int x, int y);
void moveMouse(Mouse* mouse, int dx, int dy);
void pressMouse(Mouse* mouse, u_int32_t button);
void releaseMouse(Mouse* mouse, u_int32_t button);
void closeMouse(Mouse* mouse);

#endif