#include "mouse.h"

Mouse* getMouse()
{
    Mouse* mouse = malloc(sizeof(Mouse));
    mouse->display = XOpenDisplay(NULL);

    mouse->root = DefaultRootWindow(mouse->display);
}

void setMousePos(Mouse* mouse, int x, int y)
{
    XWarpPointer(mouse->display, None, DefaultRootWindow(mouse->display), 0, 0, 0, 0, x, y);
    XFlush(mouse->display);
}

void moveMouse(Mouse* mouse, int dx, int dy)
{
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    Window child;

    XQueryPointer(mouse->display, mouse->root, &(mouse->root), &child, &root_x, &root_y, &win_x, &win_y, &mask);
    XWarpPointer(mouse->display, None, DefaultRootWindow(mouse->display), 0, 0, 0, 0, root_x + dx, root_y + dy);
    XFlush(mouse->display);
}

void pressMouse(Mouse* mouse, u_int32_t button)
{
    XTestFakeButtonEvent(mouse->display, button, True, CurrentTime);
    XFlush(mouse->display);
}

void releaseMouse(Mouse* mouse, u_int32_t button)
{
    XTestFakeButtonEvent(mouse->display, button, False, CurrentTime);
    XFlush(mouse->display);
}

void closeMouse(Mouse* mouse)
{
    XCloseDisplay(mouse->display);
    free(mouse);
}