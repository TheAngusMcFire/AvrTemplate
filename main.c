#include <avr/io.h>

int main(void)
{
    DDRB  |= _BV(5);
    PORTB |= _BV(5);
    return 0;
}