#ifndef __DRV_LED_PWM_H
#define __DRV_LED_PWM_H

#include "stm32g4xx_hal.h"

typedef struct h_led_pwm_struct
{
    TIM_HandleTypeDef * timer_handle;
    uint32_t timer_channel;
    uint8_t brightness;
} h_led_pwm_t;

// Starts the timer, put the brightness to minimum value
int8_t led_pwm_init(h_led_pwm_t * h_led_pwm);

// brightness between 0 and 1
void led_pwm_set_brightness(h_led_pwm_t * h_led_pwm, uint8_t brightness);

#endif //__DRV_LED_PWM_H