#define _POSIX_C_SOURCE 199309L
#include "timer.h"

#define TARGET_FPS 60
#define NS_PER_SEC 1000000000L
#define FRAME_DELAY_NS (NS_PER_SEC / TARGET_FPS)

void timerStart(struct timespec* startTime)
{
   clock_gettime(CLOCK_MONOTONIC, startTime); 
}

void waitForFrame(struct timespec* startTime)
{
    struct timespec end_time, sleep_time;
    
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    long elapsed_ns = (end_time.tv_sec - startTime->tv_sec) * NS_PER_SEC + 
                     (end_time.tv_nsec - startTime->tv_nsec);

    long remaining_ns = FRAME_DELAY_NS - elapsed_ns;

    if (remaining_ns > 0) {
        sleep_time.tv_sec = remaining_ns / NS_PER_SEC;
        sleep_time.tv_nsec = remaining_ns % NS_PER_SEC;
        nanosleep(&sleep_time, NULL);
    }
}