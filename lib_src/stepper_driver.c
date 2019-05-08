
#include <string.h>
#include <stepper_driver.h>


static StepperDriverInfo stepper_instaces[NUM_STEPPER_INSTANCES];

int16_t turnStepperMotor(uint16_t stepper_id, uint8_t direction, uint32_t num_steps)
{
    if (stepper_id >= NUM_STEPPER_INSTANCES) return -1;

    StepperDriverInfo* stepper_inst = &(stepper_instaces[stepper_id]);

    if(direction == DIR_LEFT)
        *(stepper_inst->port_direction) |= (1 << stepper_inst->pin_direction);
    else if (direction == DIR_RIGHT)
        *(stepper_inst->port_direction) &= ~(1 << stepper_inst->pin_direction);
    else 
        return -1;

    while(num_steps--)
    {
        *(stepper_inst->port_steps) |= (1 << stepper_inst->pin_steps);
        STEP_DELAY;
        *(stepper_inst->port_steps) &= ~(1 << stepper_inst->pin_steps);
        STEP_DELAY;
    }
    
    return 0;
}

int16_t getNewStepperMotor(StepperDriverInfo* stepper_driver_info)
{
    uint16_t stepper_id = 0;
    StepperDriverInfo* stepper_inst = 0;

    for (stepper_id = 0; stepper_id < NUM_STEPPER_INSTANCES; stepper_id++)    
        if(stepper_instaces[stepper_id].ddr_direction == 0)
            stepper_inst = &(stepper_driver_info[stepper_id]);
    
    if (stepper_inst == 0) {return -1;}
    
    memcpy(stepper_inst, stepper_driver_info, sizeof(StepperDriverInfo));
    
    *(stepper_driver_info->ddr_direction) |= (1 << stepper_driver_info->pin_direction);
    *(stepper_driver_info->ddr_steps)     |= (1 << stepper_driver_info->pin_steps);

    return stepper_id;
}