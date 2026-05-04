#include "screen.h"

ScreenStruct *initScreen(int screenWidth, int screenHeight, int xOffset, int yOffset)
{
    ScreenStruct *screenStruct = malloc(sizeof(ScreenStruct));
    screenStruct->display = XOpenDisplay(NULL);
    screenStruct->root = DefaultRootWindow(screenStruct->display);

    screenStruct->screenWidth = screenWidth;
    screenStruct->screenHeight = screenHeight;
    screenStruct->xOffset = xOffset;
    screenStruct->yOffset = yOffset;

    return screenStruct;
}

char *getScreenData(ScreenStruct *screenStruct)
{
    screenStruct->image = XGetImage(screenStruct->display, screenStruct->root, screenStruct->xOffset, screenStruct->yOffset, screenStruct->screenWidth, screenStruct->screenHeight, AllPlanes, ZPixmap);

    XFixesCursorImage *cursor = XFixesGetCursorImage(screenStruct->display);

    if (cursor)
    {
        int cx = cursor->x - screenStruct->xOffset;
        int cy = cursor->y - screenStruct->yOffset;

        for (int y = 0; y < cursor->height; y++)
        {
            for (int x = 0; x < cursor->width; x++)
            {
                unsigned long pixel = cursor->pixels[y * cursor->width + x];

                int px = cx + x - cursor->xhot;
                int py = cy + y - cursor->yhot;

                if (px >= 0 && px < screenStruct->screenWidth && py >= 0 && py < screenStruct->screenHeight)
                {
                    // Ignorar transparencia total
                    if ((pixel >> 24) == 0)
                        continue;

                    XPutPixel(screenStruct->image, px, py, pixel);
                }
            }
        }

        XFree(cursor);
    }

    return screenStruct->image->data;
}

void closeScreen(ScreenStruct *screenStruct)
{
    XDestroyImage(screenStruct->image);
    XCloseDisplay(screenStruct->display);
    free(screenStruct);
}