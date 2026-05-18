#ifndef VIRT_KEYBOARD_H
#define VIRT_KEYBOARD_H

int initVirtualKeyboard(const char* deviceName);
void pressCharacter(int fd, char c);
void releaseCharacter(int fd, char c);
void typeCharacter(int fd, char c);
void typeString(int fd, const char *str);
void closeVirtualKeyboard(int fd);

#endif