#include <etc.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <uart.h>
#include <adc.h>
#include <timer.h>
#include <pin_int.h>

void uart_rcv_event()
{
    uartWriteString("\nThis is the UART interrupt handler" "\n");
    adcStartMeasurement(0 /* just use channel 0 */);
}

void adc_fin_event(uint16_t adc_value)
{
    uartWriteString("\nThis is the ADC interrupt handler" "\n");
}

void timer_ov_interrupt()
{
    PORTB ^= _BV(5);
    timerSetValue(57724); //overflow in 500ms
}

void pin_int0_interrupt()
{
    uartWriteString("\npin0 int triggered\n");
}

void pin_int1_interrupt()
{
    uartWriteString("\npin1 int triggered\n");
}

void init()
{
    //switch on the integrated LED
    DDRB  |= _BV(5);
    PORTB |= _BV(5);

    uartInit(103);
    adcInit();
    timerInit();

    adcRegisterInterrupt(adc_fin_event);
    uartRegisterInterrupt(uart_rcv_event);
    timerRegisterInterrupt(timer_ov_interrupt);
    pinInt0RegisterInterrupt(pin_int0_interrupt, INT_FALLING_EDGE);
    pinInt1RegisterInterrupt(pin_int1_interrupt, INT_FALLING_EDGE);

    sei();
}

int main(void)
{
    init();

    uartWriteString("Hello this is the " WHO_AM_I "\n");

    while(1);

    return 0;
}