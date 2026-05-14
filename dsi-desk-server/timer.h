#ifndef TIMER_H
#define TIMER_H

#include <time.h>

void timerStart(struct timespec* startTime);
void waitForFrame(struct timespec* startTime);

#endif