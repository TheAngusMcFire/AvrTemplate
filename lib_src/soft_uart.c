#include <etc.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <soft_uart.h>
#include <pin_int.h>

static void (*soft_uart_rcv_int_funct)() = 0;
static uint8_t half_bit_duration = 0;

#define HALF_BIT_WAIT_TIME delayUs(half_bit_duration)
#define SINGLE_BIT_WAIT_TIME \
    HALF_BIT_WAIT_TIME;      \
    HALF_BIT_WAIT_TIME;

static void __attribute__((optimize("O4"))) delayUs(uint8_t delay)
{
    delay--;
    do
    {
        __asm volatile("nop\n nop\n nop\n nop\n");
        __asm volatile("nop\n nop\n nop\n nop\n");
        __asm volatile("nop\n nop\n nop\n nop\n");
    } while (delay--);
}

void softUartPinInterrupt()
{
    if (soft_uart_rcv_int_funct != 0)
        soft_uart_rcv_int_funct();
}

void softUartRegisterInterrupt(void (*ptr)())
{
#ifdef SOFT_UART_RXPD2_TXPD5
    pinInt0RegisterInterrupt(softUartPinInterrupt, INT_FALLING_EDGE);
#else
    pinInt1RegisterInterrupt(softUartPinInterrupt, INT_FALLING_EDGE);
#endif
    soft_uart_rcv_int_funct = ptr;
}

uint8_t __attribute__((optimize("O4"))) softUartGetChar()
{
    uint8_t input_value = 0;
    while (INPUT_PIN_HIGH)
        ;
    SINGLE_BIT_WAIT_TIME;

    for (uint8_t bit = 0; bit < 8; bit++)
    {
        HALF_BIT_WAIT_TIME;
        if (INPUT_PIN_HIGH)
            input_value += (1 << bit);
        HALF_BIT_WAIT_TIME;
    }
    HALF_BIT_WAIT_TIME;

    return input_value;
}

void __attribute__((optimize("O4"))) softUartWriteChar(uint8_t chr)
{
    OUTPUT_PIN_LOW;

    SINGLE_BIT_WAIT_TIME;
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        if (chr & _BV(bit))
        {
            OUTPUT_PIN_HIGH;
        }
        else
        {
            OUTPUT_PIN_LOW;
        }

        SINGLE_BIT_WAIT_TIME;
    }

    OUTPUT_PIN_HIGH;
    SINGLE_BIT_WAIT_TIME;
}

void softUartInit(uint8_t bit_duration)
{
    half_bit_duration = bit_duration / 2;

#ifdef SOFT_UART_RXPD2_TXPD5
    DDRD |= _BV(PD5);
    DDRD &= ~_BV(PD2);
#else
    DDRD |= _BV(PD4);
    DDRD &= ~_BV(PD3);
#endif
}
