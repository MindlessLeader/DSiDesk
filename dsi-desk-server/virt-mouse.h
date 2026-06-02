#ifndef VIRT_MOUSE_H
#define VIRT_MOUSE_H

int getMouseFd(const char* deviceName);
void setMouseLeft(int fd, int val);
void moveMouse(int fd, int x, int y);
void setMouseRight(int fd, int val);
void updateMouse(int fd);
void closeMouse(int fd);

#endif