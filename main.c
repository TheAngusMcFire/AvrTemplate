#include <etc.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <uart.h>
#include <adc.h>
#include <timer.h>
#include <pin_int.h>
#include <soft_uart.h>
#include <ir.h>

int usart_putchar_printf(char var, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

int usart_putchar_printf(char var, FILE *stream) 
{
    (void)stream;
    if (var == '\n') uartWriteChar('\r');
    uartWriteChar(var);
    return 0;
}

void uart_rcv_event()
{
    uartWriteString("\nThis is the UART interrupt handler"
                    "\n");
    adcStartMeasurement(0 /* just use channel 0 */);
}

void adc_fin_event(uint16_t adc_value)
{
    (void)adc_value;
    uartWriteString("\nThis is the ADC interrupt handler"
                    "\n");
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

void soft_uart_interrupt()
{
    uint8_t data = softUartGetChar();
    softUartWriteChar(data);
    uartWriteChar(data);
}

void init()
{
    //switch on the integrated LED
    DDRB |= _BV(5);
    PORTB |= _BV(5);

    uartInit(103);
    stdout = &mystdout;
    adcInit();
    timerInit();
    //the softuart is tested for speeds up to 38400 baut 
    softUartInit(SOFT_UART_BAUT_TO_US(9600));

    adcRegisterInterrupt(adc_fin_event);
    uartRegisterInterrupt(uart_rcv_event);
    timerRegisterInterrupt(timer_ov_interrupt);
    softUartRegisterInterrupt(soft_uart_interrupt);
    //those are used by the soft uart
    //pinInt0RegisterInterrupt(pin_int0_interrupt, INT_FALLING_EDGE);
    //pinInt1RegisterInterrupt(pin_int1_interrupt, INT_FALLING_EDGE);

    sei();
}

int main(void)
{
    init();

    printf("Hello this is the " WHO_AM_I "\n");

    while (1)
    {

    }

    return 0;
}
