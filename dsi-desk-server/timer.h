#ifndef TIMER_H
#define TIMER_H

#include <time.h>

#define NS_PER_SEC 1000000000L

void timerStart(struct timespec* startTime);
long getTimePassed(struct timespec* startTime);
void waitForFrame(struct timespec* startTime);

#endif