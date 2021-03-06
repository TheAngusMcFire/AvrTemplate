#include <etc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <timer.h>

static void (*timer_overflow_int_funct) () = 0;

ISR(TIMER1_OVF_vect) 
{
    if(timer_overflow_int_funct != 0)
        timer_overflow_int_funct();
}

void timerInit()
{
    timerStart();
}

void timerStart()
{
    TCCR1B = TIMER_CLK_DIV1024;
}

void timerStop()
{
    TCCR1B = TIMER_CLK_STOP;
}

void timerSetValue(uint16_t value)
{
    TCNT1 = value;
}

uint16_t timerGetValue()
{
    return TCNT1;
}

void timerRegisterInterrupt(void (*ptr) ())
{
    timer_overflow_int_funct = ptr;
    TIMSK1 |= _BV(TOIE1);
}
