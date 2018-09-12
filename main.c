#include <etc.h>
#include <avr/io.h>
#include <uart.h>

void init()
{
    //switch on the integrated LED
    DDRB  |= _BV(5);
    PORTB |= _BV(5);

    uartInit(103);
}

int main(void)
{
    init();

    uartWriteString("Hello this is the " WHO_AM_I "\n");

    while(1);

    return 0;
}