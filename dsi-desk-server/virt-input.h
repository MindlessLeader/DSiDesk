#ifndef VIRT_INPUT_H
#define VIRT_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>

void emit(int fd, int type, int code, int val);

#endif