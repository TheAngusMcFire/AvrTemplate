#include <avr/io.h>
#include <avr/interrupt.h>

#include <uart.h>

void (*uart_rcv_int_funct) () = 0;

void registerInterrupt(void (*ptr) ())
{
    uart_rcv_int_funct = ptr;
}

ISR(USART_RX_vect)
{
    if(uart_rcv_int_funct != 0)
        uart_rcv_int_funct();
}


void uartInit()
{

}

void uartWriteChar(char chr)
{

}

void uartWriteString(char* str)
{

}

char uartGetChar()
{
    return 0;
}

char uartGetCharTimeOut(uint16_t timeout)
{
    return 0;
}