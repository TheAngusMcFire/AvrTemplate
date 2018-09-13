#ifndef PIN_INT_H
#define PIN_INT_H

enum PIN_INT_SETTINGS
{
    PIN_NONE = 0,
    PIN_INT0_INT1 = 1,
    PIN_INT0 = 2,
    PIN_INT1 = 3,
};

enum PIN_CHANGES
{
    INT_ON_LOW = 0,
    INT_ANY_CHANGE = 1,
    INT_FALLING_EDGE = 2,
    INT_RISING_EDGE = 3
};

void pinInt0RegisterInterrupt(void (*ptr)(), uint8_t pin_change_setting);
void pinInt1RegisterInterrupt(void (*ptr)(), uint8_t pin_change_setting);

#endif