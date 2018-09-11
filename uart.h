
#ifndef UART_H
#define UART_H

#include <stdint.h>

void registerInterrupt(void (*ptr) ());

void uartInit();
void uartWriteChar(char chr);
void uartWriteString(char* str);

char uartGetChar();
char uartGetCharTimeOut(uint16_t timeout);

#endif