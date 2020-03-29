
#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UBBR_VALUE(baut_rate) ((F_CPU / (16UL * baut_rate)) -1UL)

//minus 4 clocks because the 16 bit decrement
#define ONE_US_AT16MHZ \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \
        __asm volatile ("nop"); \


void uartRegisterInterrupt(void (*ptr) ());

void uartInit(uint16_t);
void uartWriteChar(char chr);
void uartWriteString(const char* str);

char uartGetChar();
char uartGetCharTimeOut(uint16_t timeout);

#endif