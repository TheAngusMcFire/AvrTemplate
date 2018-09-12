#ifndef ADC_H
#define ADC_H

#include <stdint.h>

enum ADC_DIVIDER 
{
    ADC_CLK_DIV2 = 1,
    ADC_CLK_DIV4 = 2,
    ADC_CLK_DIV8 = 3,
    ADC_CLK_DIV16 = 4,
    ADC_CLK_DIV32 = 5,
    ADC_CLK_DIV64 = 6,
    ADC_CLK_DIV128 = 7
};

void adcInit();
void adcStartMeasurement(uint8_t channel);
uint16_t adcGetValue(uint8_t channel);
void adcRegisterInterrupt(void (*ptr) (uint16_t));

#endif