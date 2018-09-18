#include <etc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <adc.h>

static void (*adc_fin_int_funct) (uint16_t) = 0;

void adcRegisterInterrupt(void (*ptr) (uint16_t))
{
    //enable interrupt
    ADCSRA |= _BV(ADIE);
    adc_fin_int_funct = ptr;
}

ISR(ADC_vect)
{
    if(adc_fin_int_funct != 0)
        adc_fin_int_funct(ADCW);
}   

void adcInit()
{
    //the adc frequency must be between 50khz and 200khz
    uint8_t prescaler = ADC_CLK_DIV128;

    //set the ref here because a ref change takes time!!!
    ADMUX = _BV(REFS0);
    ADCSRA = _BV(ADEN) | prescaler;
}

void adcStartMeasurement(uint8_t channel)
{
    ADMUX = _BV(REFS0) | (channel & 0x0F);
    ADCSRA |= _BV(ADSC);
}

uint16_t adcGetValue(uint8_t channel)
{
    ADMUX = _BV(REFS0) | (channel & 0x0F);
    ADCSRA |= _BV(ADSC);

    while (ADCSRA & _BV(ADSC))
        __asm volatile("nop");
    
    return ADCW;
}