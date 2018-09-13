
//this is just to make sure VS code does not highlight all of the SFRs and stuff

//change this of differend CPUs otherwise it will not build
#ifndef ETC_H
#define ETC_H

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef F_CPU
#define F_CPU 16000000
#endif

#ifndef WHO_AM_I
#define WHO_AM_I "TEMPLATE"
#endif


enum IO_PORTS
{
    PORTB = 0,
    PORTC = 1,
    PORTD = 2
};

#endif