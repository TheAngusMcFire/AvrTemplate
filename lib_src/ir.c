#include <etc.h>
#include <ir.h>
#include <avr/io.h>

static uint16_t pulses[PULSES_BUFFER_SIZE];
static uint16_t pulses_cnt   = 0;
static uint8_t num_enc_data = 0;

uint16_t OPTIMIZE_O4 wait_for_high_value(uint16_t max_value)
{
    uint16_t time_elapsed = 0;

    while((IR_RCV_PORT & IR_RCV_PIN) == 0 && time_elapsed < max_value)
    {    
        __asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n");  
        time_elapsed++;
    }

    return time_elapsed;
}

uint16_t OPTIMIZE_O4 wait_for_low_value(uint16_t max_value)
{
    uint16_t time_elapsed = 0;

    while((IR_RCV_PORT & IR_RCV_PIN) != 0 && time_elapsed < max_value)
    {    
        __asm volatile("nop\n nop\n nop\n nop\n nop\n nop\n");  
        time_elapsed++;
    }

    return time_elapsed;
}

uint16_t OPTIMIZE_O4 wait_for_high()
{
    uint16_t time_elapsed = 0;

    while((IR_RCV_PORT & IR_RCV_PIN) == 0)
    {       
        wait_us();   
        time_elapsed++;
    }

    return time_elapsed;
}

uint16_t OPTIMIZE_O4 wait_for_low()
{
    uint16_t time_elapsed = 0;

    while((IR_RCV_PORT & IR_RCV_PIN) != 0)
    {       
        wait_us();   
        time_elapsed++;
    }

    return time_elapsed;
}

void wait_for_start_frame()
{
    uint16_t first_pulse = 0;
    uint16_t second_pulse = 0;

    do
    {   
        (void)wait_for_low();
        first_pulse = wait_for_high();
        second_pulse = wait_for_low();

        uint16_t tmp = first_pulse - second_pulse;
        if(tmp < (second_pulse + THRESHOLD) && tmp > (second_pulse - THRESHOLD))
            break;

    }while(1);
}

uint8_t isPulse(uint16_t pulse, uint16_t min, uint16_t threshold)
{
    if(pulse > (min - threshold) && pulse < (min + threshold))
        return 1;
    else 
        return 0;
}

uint8_t decode_frames(uint8_t* buff, uint16_t max, uint16_t min, uint16_t number)
{
    (void)max;

    if(number % 8 != 0)
        return pulses_cnt;

    uint32_t typical_long = 0;
    uint32_t typical_short = 0;
    
    uint16_t num_long = 0;
    uint16_t num_short = 0;
    uint16_t min_long = 0xffff;
    uint16_t max_long = 0;

    uint16_t min_short = 0xffff;
    uint16_t max_short = 0;
    uint16_t threshhold_long = 0;

    for(uint16_t index = 0;index < number; index++)
    {
        uint16_t tmp = pulses[index];

        if((tmp / min) > 1)
        {
            num_long++;
            if(tmp < min_long) min_long = tmp;
            if(tmp > max_long) max_long = tmp;
            typical_long += tmp;
        }
        else
        {
            num_short++;
            if(tmp < min_short) min_short = tmp;
            if(tmp > max_short) max_short = tmp;
            typical_short += tmp;
        }        
    }

    typical_long /= num_long;
    typical_short /= num_short;

    threshhold_long = typical_long * 0.25;

    uint8_t num_bytes = number / 8;
    uint16_t pulses_processed = 0;

    for(num_enc_data = 0; num_enc_data < num_bytes; num_enc_data++)
    {
        uint8_t tmp = 0;
        for(uint8_t bits = 0; bits < 8; bits++)
        {
            uint8_t pulse = isPulse(pulses[pulses_processed++],typical_long,threshhold_long);
          
            tmp |= (pulse << bits);
        }
        buff[num_enc_data] = tmp;
    }

    uint16_t checksum = 0;

    for(uint8_t i = 0; i < num_enc_data - 1; i++)    
        checksum += buff[i];    

    checksum &= 0xff;

    return 0;
}

uint8_t ir_get_frame(uint8_t* buff)
{
    uint16_t max_high_value = 0;
    uint16_t min_high_value = 0xffff;
    uint16_t tmp = 0;
    uint16_t wait_value = 0;
    pulses_cnt = 0;

    wait_for_start_frame();
    (void)wait_for_high();

    do
    {
        wait_value = (min_high_value != 0xffff ? min_high_value * 5 : 0xFFFF);
        tmp = wait_for_low_value(wait_value);

        if(tmp == wait_value) break;

        pulses[pulses_cnt++] = tmp;

        if(tmp > max_high_value) max_high_value = tmp;
        if(tmp < min_high_value) min_high_value = tmp;

        tmp = wait_for_high_value(max_high_value + THRESHOLD);

        if(tmp == max_high_value + THRESHOLD) break;
    }while(1);

    return decode_frames(buff, max_high_value, min_high_value, pulses_cnt);
}

uint8_t ir_get_num_decoded_bytes()
{
    return num_enc_data;
}

void ir_init()
{
    DDRB &= ~IR_RCV_PIN;
    PORTB |= IR_RCV_PIN;

    DDRB |= IR_TRX_PIN;
}

static void __attribute__((optimize("O4"))) delayUs(uint16_t delay)
{
    delay--;
    do
    {
        __asm volatile("nop\n nop\n nop\n nop\n");
        __asm volatile("nop\n nop\n nop\n nop\n");
        __asm volatile("nop\n nop\n nop\n nop\n");
    } while (delay--);
}




void OPTIMIZE_O4 ir_write_pulse(uint16_t num_pulses)
{
    for(uint16_t pulse = 0; pulse < num_pulses - 1; pulse++)
    {
        IR_TRX_PORT |= IR_TRX_PIN;
        IR_HIGH_PULSE;
        IR_TRX_PORT &= ~IR_TRX_PIN;
        IR_LOW_PULSE;
    }

    IR_TRX_PORT |= IR_TRX_PIN;
    IR_HIGH_PULSE;
    IR_TRX_PORT &= ~IR_TRX_PIN;
}

void OPTIMIZE_O4 write_byte(uint8_t byte)
{
    for(uint8_t bit = 0; bit < 8; bit++)
    {
        uint8_t high = (byte & (1 << bit)) != 0 ? 1 : 0;
        ir_write_pulse(HIGH_PULSE_DELAY);

        if(high)
            delayUs(LOW_DELAY_LONG);
        else
            delayUs(LOW_DELAY_SHORT);
    }  
}

void  OPTIMIZE_O4 ir_send_frame(uint8_t* buff, uint8_t size)
{
    {   //start frame
        ir_write_pulse(START_LONG);
        delayUs(START_SHORT);
    }

    for(int index = 0; index < size; index++ )    
        write_byte(buff[index]);

    ir_write_pulse(HIGH_PULSE_DELAY);    
}

void OPTIMIZE_O4 writeInitFrame()
{    
    ir_write_pulse(HIGH_PULSE_DELAY);
    delayUs(500);

    ir_write_pulse(HIGH_PULSE_DELAY);
    delayUs(500);

    ir_write_pulse(HIGH_PULSE_DELAY);
    delayUs(500);

    ir_write_pulse(HIGH_PULSE_DELAY);
    delayUs(500);

    ir_write_pulse(HIGH_PULSE_DELAY);
    delayUs(500);

    ir_write_pulse(HIGH_PULSE_DELAY);
}