#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

enum TIMER_DIVIDER 
{
    TIMER_CLK_STOP = 0,
    TIMER_CLK_DIV1 = 1,
    TIMER_CLK_DIV8 = 2,
    TIMER_CLK_DIV64 = 3,
    TIMER_CLK_DIV256 = 4,
    TIMER_CLK_DIV1024 = 5,
    TIMER_CLK_EXT_FALLING = 6,
    TIMER_CLK_EXT_RISING = 7
};


extern inline void timerStart();
extern inline void timerStop();
extern inline void timerSetValue(uint16_t value);

void timerInit();
void timerSetValue(uint16_t value);
void timerRegisterInterrupt(void (*ptr) ());

#endif