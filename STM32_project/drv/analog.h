#ifndef __DRV_ANALOG_H
#define __DRV_ANALOG_H

#include "stm32g4xx_hal.h"

typedef enum adc_index_enum
{
    ANALOG_ADC_X_PARAM_INDEX, 
    ANALOG_ADC_Y_PARAM_INDEX, 
    ANALOG_ADC_X_ATTV_INDEX,
    ANALOG_ADC_X_INPUT_INDEX,
    ANALOG_ADC_Y_ATTV_INDEX,
    ANALOG_ADC_Y_INPUT_INDEX,
    ANALOG_ADC_PITCH_INPUT_INDEX,
    ANALOG_ADC_UNUSED_INDEX,
    ANALOG_ADC_BUFFER_LENGTH 
} analog_adc_index_t;

// HAL doc says it's bytes, but once again, ST is lying, it's words. Divide by 2 because adc_buffer is uint16_t
#define ANALOG_ADC_BUFFER_LENGTH_WORDS (ANALOG_ADC_BUFFER_LENGTH / 2)
#define ANALOG_ADC_NUMBER 2

#define DAC_BUFFER_SIZE 256

typedef struct h_analog_struct
{
    ADC_HandleTypeDef * adc_handles[ANALOG_ADC_NUMBER];
    TIM_HandleTypeDef * adc_timer_handle;
    DAC_HandleTypeDef * dac_handle;
    TIM_HandleTypeDef * dac_timer_handle;
    uint32_t dac_channel;

    uint16_t adc_buffer[ANALOG_ADC_BUFFER_LENGTH];
//    uint16_t dac_value;
    uint32_t dac_buffer[DAC_BUFFER_SIZE];
} h_analog_t;

int8_t analog_init(h_analog_t * h_analog);
uint16_t analog_get_adc(h_analog_t * h_analog, analog_adc_index_t index);
//void analog_set_dac(h_analog_t * h_analog, uint16_t value);

#endif //__DRV_ANALOG_H