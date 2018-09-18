#include <etc.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <uart.h>

static void (*uart_rcv_int_funct) () = 0;
static uint8_t uart_get_char_timeout = 0x55;

uint8_t uartGetTimeOutState()
{
    if(uart_get_char_timeout == 0xAA)
    {
        //reset the timeout state
        uart_get_char_timeout = 0x55; 
        return 1;
    }
    else
    {
        return 0;
    }
}

void uartRegisterInterrupt(void (*ptr) ())
{
    //enable interrupt
    UCSR0B |=  _BV(RXCIE0);
    uart_rcv_int_funct = ptr;
}

ISR(USART_RX_vect)
{
    if(uart_rcv_int_funct != 0)
        uart_rcv_int_funct();

    (void)UDR0; //clear interrupt if not handled in isr    
}


void uartInit(uint16_t ubrr_value)
{
    UBRR0 = ubrr_value;
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
}

void uartWriteChar(char chr)
{
    while (!(UCSR0A & _BV(UDRE0)))
        __asm volatile ("nop");
        
    UDR0 = chr;
}

void uartWriteString(const char* str)
{
    while(*str)
        uartWriteChar(*str++);
}

char uartGetChar()
{  
    while (!(UCSR0A & _BV(RXC0)))
        __asm volatile ("nop");
    
    return UDR0;
}

char uartGetCharTimeOut(uint16_t timeout)
{
    while (!(UCSR0A & _BV(RXC0)) && timeout)
    {
       ONE_US_AT16MHZ;
       timeout--;
    }
    
    //check for timeout
    if(timeout == 0)
    {
       uart_get_char_timeout = 0xAA;
       return 0;
    }
    else
    {
       return UDR0;
    }    
}