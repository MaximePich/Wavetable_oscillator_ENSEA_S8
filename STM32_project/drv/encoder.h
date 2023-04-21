#ifndef __DRV_ENCODER_H
#define __DRV_ENCODER_H

#include <stdint.h>

#include "stm32g4xx_hal.h"

typedef struct h_encoder_struct
{
    GPIO_TypeDef * A_GPIOx;
    uint16_t A_GPIO_pin;
    GPIO_TypeDef * B_GPIOx;
    uint16_t B_GPIO_pin;
    uint8_t state;
    int32_t value;
} h_encoder_t;

void encoder_init(h_encoder_t * h_encoder);
void encoder_set(h_encoder_t * h_encoder, int32_t value);
void encoder_process(h_encoder_t * h_encoder);
int32_t encoder_increment(h_encoder_t * h_encoder);
int32_t encoder_value(h_encoder_t * h_encoder);

#endif // __DRV_ENCODER_H