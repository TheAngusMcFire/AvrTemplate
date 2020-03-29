#include <spi.h>
#include <avr/io.h>


//http://www.tinkerer.eu/AVRLib/SPI/
void spiInit()
{
    DDRB |= _BV(PORTB3) | _BV(PORTB5);

    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
            (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));

    SPSR = (1<<SPI2X);
}

uint8_t spiExchange(uint8_t data)
{
    SPDR = data;
    while((SPSR & (1<<SPIF))==0);
    return SPDR;
}