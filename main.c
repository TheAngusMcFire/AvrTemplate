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
#include <spi.h>
#include <lora.h>

int usart_putchar_printf(char var, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

int usart_putchar_printf(char var, FILE *stream) 
{
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

uint8_t buffer[256];
void initLora()
{
    DDRB |= _BV(PB2) | _BV(PB1);
    void setSS(uint8_t state){if(state){PORTB |= _BV(PB2);}else{PORTB &= ~_BV(PB2);}}
    void setRst(uint8_t state){if(state){PORTB |= _BV(PB1);}else{PORTB &= ~_BV(PB1);}}
    spiInit();

    intCallbackHandle hndl = loraInit(&spiExchange, &setSS, &setRst, 866E6, buffer);

    if(hndl == 0)
        printf("Error while initializing lora\n");
    else
        pinInt0RegisterInterrupt(hndl, INT_RISING_EDGE);
}

void init()
{
    //switch on the integrated LED
    DDRB |= _BV(5);
    PORTB |= _BV(5);

    uartInit(UBBR_VALUE(57600));
    stdout = &mystdout;
    initLora();


    //adcInit();
    //timerInit();

    //the softuart is tested for speeds up to 38400 baut 
    //softUartInit(SOFT_UART_BAUT_TO_US(9600));

    //adcRegisterInterrupt(adc_fin_event);
    //uartRegisterInterrupt(uart_rcv_event);
    //timerRegisterInterrupt(timer_ov_interrupt);
    //softUartRegisterInterrupt(soft_uart_interrupt);
    //those are used by the soft uart
    //pinInt0RegisterInterrupt(pin_int0_interrupt, INT_FALLING_EDGE);
    //pinInt1RegisterInterrupt(pin_int1_interrupt, INT_FALLING_EDGE);

    sei();
}
void loraWritePacket(uint8_t* data, uint8_t size);
uint8_t loraReadPacket(uint8_t* data);
void loraDoStuff();
int main(void)
{
    init();

    printf("Hello this is the " WHO_AM_I "\n");

    //loraDoStuff();
    char* tst = "this is a test";

    while (1)
    {

            loraWritePacket(tst, 10);

        int len = loraReadPacket(buffer);
        //printf("test7\n");

        for(int idx = 0; idx < len; idx++)
        {
            printf("%c", buffer[idx]);
        }

        printf("\n");
        _delay_ms(5000);
    }

    return 0;
}
