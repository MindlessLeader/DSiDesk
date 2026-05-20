#ifndef VIRT_TOUCH_H
#define VIRT_TOUCH_H

int getTouchFd(int scrMaxX, int scrMaxY, const char* deviceName);
void simTouch(int fd, int x, int y);
void simTouchRelease(int fd);
void closeTouch(int fd);

#endif