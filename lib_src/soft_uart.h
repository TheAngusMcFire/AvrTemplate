#include <stdint.h>
#include <etc.h>

#ifndef SOFT_UART_H
#define SOFT_UART_H

#define SOFT_UART_BAUT_TO_US(baut_rate) (uint8_t)((1.0/(float)baut_rate) * 1000000.0)

#define SOFT_UART_RXPD2_TXPD5

#ifdef SOFT_UART_RXPD2_TXPD5
#define OUTPUT_PIN_LOW  PORTD &= ~_BV(PD5);
#define OUTPUT_PIN_HIGH  PORTD |= _BV(PD5); 
#define INPUT_PIN_HIGH bit_is_set(PIND, PD2)
#else
#define OUTPUT_PIN_LOW  PORTD &= ~_BV(PD4);
#define OUTPUT_PIN_HIGH  PORTD |= _BV(PD4);
#define INPUT_PIN_HIGH bit_is_set(PIND, PD3)
#endif


void softUartRegisterInterrupt(void (*ptr)());
uint8_t softUartGetChar();
void softUartWriteChar(uint8_t chr);
void softUartInit(uint8_t bit_duration);

#endif