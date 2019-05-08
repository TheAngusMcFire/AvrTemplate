#ifndef LIB_SRC_IR_H
#define LIB_SRC_IR_H

#include <stdint.h>

#define KLIMA_WORK

//421
//1311
//445

#ifdef KLIMA_WORK
#define HIGH_PULSE_DELAY 16
#define LOW_DELAY_LONG  1311
#define LOW_DELAY_SHORT 445
#define START_LONG 129
#define START_SHORT 1745
#endif

#ifdef FERNSEHR
#define HIGH_PULSE_DELAY 23
#define LOW_DELAY_LONG  1629
#define LOW_DELAY_SHORT 501
#define START_LONG 336
#define START_SHORT 4482
#endif

#define IR_ONE_US_NOPS __asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n"); 
#define IR_TWO_US_NOPS {IR_ONE_US_NOPS IR_ONE_US_NOPS }
#define IR_FOUR_US_NOPS {IR_TWO_US_NOPS IR_TWO_US_NOPS }
#define IR_EIGHT_US_NOPS {IR_FOUR_US_NOPS IR_FOUR_US_NOPS }

#define IR_HIGH_PULSE {IR_EIGHT_US_NOPS IR_FOUR_US_NOPS IR_TWO_US_NOPS IR_ONE_US_NOPS __asm volatile("nop\n nop\n nop\n ");}
#define IR_LOW_PULSE  {IR_EIGHT_US_NOPS IR_TWO_US_NOPS IR_ONE_US_NOPS __asm volatile("nop\n"); }

#define wait_us()\
{\
    __asm volatile("nop\n nop\n nop\n nop\n");\
    __asm volatile("nop\n nop\n nop\n nop\n");\
    __asm volatile("nop\n nop\n nop\n");\
}\

#define IR_RCV_PORT PINB
#define IR_RCV_PIN  _BV(4)

#define IR_TRX_PORT PORTB
#define IR_TRX_PIN  _BV(1)

#define OPTIMIZE_O4 __attribute__((optimize("O4")))

#define THRESHOLD (500u)
#define PULSES_BUFFER_SIZE (256u)

uint8_t ir_get_frame(uint8_t* buff);
void    ir_init(void);
uint8_t ir_get_num_decoded_bytes(void);

void ir_send_frame(uint8_t* buff, uint8_t size);
void writeInitFrame(void);

#endif /* LIB_SRC_IR_H */