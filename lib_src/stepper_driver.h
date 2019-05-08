#ifndef LIB_SRC_STEPPER_DRIVER_H
#define LIB_SRC_STEPPER_DRIVER_H

#include <etc.h>
#include <stdint.h>
#include <util/delay.h>

#define NUM_STEPPER_INSTANCES 10
#define STEP_DELAY (_delay_us(10))

typedef struct
{
    volatile uint8_t* ddr_direction;
    volatile uint8_t* port_direction;
    volatile uint8_t* ddr_steps;
    volatile uint8_t* port_steps;
    uint8_t pin_direction;
    uint8_t pin_steps;
} StepperDriverInfo;

enum
{
    DIR_LEFT  = 0xAA,
    DIR_RIGHT = 0x55,
};

int16_t getNewStepperMotor(StepperDriverInfo* stepper_driver_info);
int16_t turnStepperMotor(uint16_t stepper_id, uint8_t direction, uint32_t num_steps);

#endif /* LIB_SRC_STEPPER_DRIVER_H */