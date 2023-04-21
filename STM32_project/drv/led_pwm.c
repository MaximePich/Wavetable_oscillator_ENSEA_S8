#include "led_pwm.h"

// Starts the timer, put the brightness to minimum value
int8_t led_pwm_init(h_led_pwm_t * h_led_pwm)
{
    if (HAL_OK != HAL_TIM_PWM_Start(h_led_pwm->timer_handle, h_led_pwm->timer_channel))
    {
        return -1;
    }
    led_pwm_set_brightness(h_led_pwm, 0.f);

    return 0;
}

// brightness between 0 and 1
void led_pwm_set_brightness(h_led_pwm_t * h_led_pwm, uint8_t brightness)
{
    uint16_t bright = 256 - (uint16_t)(brightness);
    __HAL_TIM_SET_COMPARE(h_led_pwm->timer_handle, h_led_pwm->timer_channel, bright);
}
