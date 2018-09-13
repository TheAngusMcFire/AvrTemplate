#include <etc.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <pin_int.h>

static void (*ext_pin0_int_funct) () = 0;
static void (*ext_pin1_int_funct) () = 0;

ISR(INT0_vect)
{
	if(ext_pin0_int_funct != 0)
        ext_pin0_int_funct();

        EIFR |= _BV(INTF0); //clear interrupt
}

ISR(INT1_vect)
{
	if(ext_pin1_int_funct != 0)
        ext_pin1_int_funct();

        EIFR |= _BV(INTF1); //clear interrupt
}

void pinInt0RegisterInterrupt(void (*ptr)(), uint8_t pin_change_setting)
{
    DDRD &= ~_BV(PD2);//input		
	PORTD |= _BV(PD2); //pullup
    
    EIMSK |= _BV(INT0);
    EICRA |= pin_change_setting;

    ext_pin0_int_funct = ptr;
}

void pinInt1RegisterInterrupt(void (*ptr)(), uint8_t pin_change_setting)
{
    DDRD &= ~_BV(PD3);//input		
	PORTD |= _BV(PD3); //pullup
    
    EIMSK |= _BV(INT1);
    EICRA |= pin_change_setting << 2;
    ext_pin1_int_funct = ptr;
}