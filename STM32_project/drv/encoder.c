#include "encoder.h"

void encoder_init(h_encoder_t * h_encoder)
{
    h_encoder->state = 0;
    h_encoder->value = 0;
}

void encoder_set(h_encoder_t * h_encoder, int32_t value)
{
    h_encoder->value = value;
}

void encoder_process(h_encoder_t * h_encoder)
{
    h_encoder->state = h_encoder->state << 1;
    if (HAL_GPIO_ReadPin(h_encoder->A_GPIOx, h_encoder->A_GPIO_pin) == GPIO_PIN_RESET)
    {
        h_encoder->state |= 1;
    }

    h_encoder->value += encoder_increment(h_encoder);
}

int32_t encoder_increment(h_encoder_t * h_encoder)
{
    uint32_t increment = 0;
    if (0x01 == h_encoder->state)
    {
        	if (HAL_GPIO_ReadPin(h_encoder->B_GPIOx, h_encoder->B_GPIO_pin) == GPIO_PIN_SET)
			{
				increment = 1;
			}
			else
			{
				increment = -1;
			}
    }

    return increment;
}

int32_t encoder_value(h_encoder_t * h_encoder)
{
    return h_encoder->value;
}