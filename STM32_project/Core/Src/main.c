/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "drv/led_pwm.h"
#include "drv/encoder.h"
#include "drv/analog.h"
#include "drv/eeprom.h"

#include "dsp/dsp.h"
#include "dsp/wave.h"

//TODOs
//Handle multiple modes wavetable/wavemixer/calibration with the encoder push button.
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct eeprom_save_struct 
{
  int32_t encoder;
  float truc;
  float machin;
} eeprom_save_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TIM_CHANNEL_BLUE TIM_CHANNEL_1
#define TIM_CHANNEL_GREEN TIM_CHANNEL_2
#define TIM_CHANNEL_RED TIM_CHANNEL_3

//#define ADC_BUFFER_LENGTH_WORDS (ADC_BUFFER_LENGTH/2)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
h_led_pwm_t h_led_red = {
  .timer_handle = &htim1,
  .timer_channel = TIM_CHANNEL_RED
};

h_led_pwm_t h_led_green = {
  .timer_handle = &htim1,
  .timer_channel = TIM_CHANNEL_GREEN
};

h_led_pwm_t h_led_blue = {
  .timer_handle = &htim1,
  .timer_channel = TIM_CHANNEL_BLUE
};

h_encoder_t h_encoder = {
  .A_GPIOx = ENCODER_A_GPIO_Port,
  .A_GPIO_pin = ENCODER_A_Pin,
  .B_GPIOx = ENCODER_B_GPIO_Port,
  .B_GPIO_pin = ENCODER_B_Pin
};

h_analog_t h_analog = {
  .adc_handles = {&hadc1, &hadc2},
  .adc_timer_handle = &htim6,
  .dac_handle = &hdac1,
  .dac_timer_handle = &htim7,
  .dac_channel = DAC_CHANNEL_1
};

h_dsp_t h_dsp = {
  .sample_frequency = 47995.48277809147f
};

static eeprom_save_t eeprom_save;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	(void) file;

	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, (uint16_t) len, HAL_MAX_DELAY);

	return len;
}

// TODO Should be placed in drv/something
void read_switch(void)
{
	static uint8_t state_SW = 0;

	if (state_SW == 0)
	{
		if (HAL_GPIO_ReadPin(ENCODER_SWITCH_GPIO_Port, ENCODER_SWITCH_Pin) == GPIO_PIN_SET)
		{
      eeprom_save.encoder = encoder_value(&h_encoder);
      printf("Saving pitch %d...\r\n", eeprom_save.encoder);
      if(eeprom_write((void*)&eeprom_save, sizeof(eeprom_save_t)) == HAL_OK)
      {
        printf("Pitch saved\r\n");
        }
      else
      {
        printf("Error saving pitch in eeprom\r\n");
      }

			state_SW = 1;
		}
	}
	else if (state_SW == 1)
	{
		if (HAL_GPIO_ReadPin(ENCODER_SWITCH_GPIO_Port, ENCODER_SWITCH_Pin) == GPIO_PIN_RESET)
		{
			state_SW = 0;
		}
	}
}

// Called every 1 ms
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  if (ADC1 == hadc->Instance)
  {
    HAL_GPIO_WritePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin, GPIO_PIN_SET);

    encoder_process(&h_encoder);
		read_switch();
    //Process analog inputs
    //Params (Potentiometers and encoder)
    uint16_t x_param        = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_X_PARAM_INDEX);
    uint16_t y_param        = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_Y_PARAM_INDEX);
    uint16_t x_attv_param   = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_X_ATTV_INDEX);
    uint16_t y_attv_param   = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_Y_ATTV_INDEX);
    int32_t encoder         = encoder_value(&h_encoder);
    h_dsp.params[X_PARAM]       = x_param;
    h_dsp.params[Y_PARAM]       = y_param;
    h_dsp.params[X_ATTV_PARAM]  = x_attv_param;
    h_dsp.params[Y_ATTV_PARAM]  = y_attv_param;
    h_dsp.params[PITCH_PARAM]   = encoder;

    //Inputs (IN jacks)
    uint16_t x_in     = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_X_INPUT_INDEX);
    uint16_t y_in     = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_Y_INPUT_INDEX); 
    uint16_t pitch    = 4095 - analog_get_adc(&h_analog, ANALOG_ADC_PITCH_INPUT_INDEX);  // C0 = 2V = 2376LSB ; C2 = 4V = 2814LSB
    h_dsp.inputs[X_INPUT] = x_in;
    h_dsp.inputs[Y_INPUT] = y_in;
    h_dsp.inputs[PITCH_INPUT] = pitch;



    led_pwm_set_brightness(&h_led_red, x_param >> 4);
    led_pwm_set_brightness(&h_led_green, encoder + 127);
    led_pwm_set_brightness(&h_led_blue, y_param >> 4);

    HAL_GPIO_WritePin(LED_DEBUG_GPIO_Port, LED_DEBUG_Pin, GPIO_PIN_RESET);
  }
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
  if (DAC1 == hdac->Instance)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);

    dsp_process(&h_dsp, h_analog.dac_buffer, DAC_BUFFER_SIZE/2);

    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_RESET);
  }
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
  if (DAC1 == hdac->Instance)
  {
    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);

    dsp_process(&h_dsp, &h_analog.dac_buffer[DAC_BUFFER_SIZE/2], DAC_BUFFER_SIZE/2);

    HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_RESET);
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  printf("===== WVTBL =====\r\n");

  encoder_init(&h_encoder);
  dsp_init(&h_dsp);
  
  if (0 != led_pwm_init(&h_led_red))
  {
    Error_Handler();
  }
  if (0 != led_pwm_init(&h_led_green))
  {
    Error_Handler();
  }
  if (0 != led_pwm_init(&h_led_blue))
  {
    Error_Handler();
  }
  if (0 != analog_init(&h_analog))
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /* TODO
  // We want to be able to calibrate the v/oct input
  // If encoder is pressed, enter calibration mode
  // Blink one color (eg. blue) while calibrating, say 1V (C2)
  // Waits second encoder press
  // We might light green LED for 1 or 2 seconds to send feedback to the user
  // Blink another color (eg. red) while calibrating, say 3V (C4)
  */
  eeprom_read((void*)&eeprom_save, sizeof(eeprom_save_t));
  printf("Encoder value: %ld\r\n", eeprom_save.encoder);
  encoder_set(&h_encoder, eeprom_save.encoder);

  while (1)
  {
#ifdef DEBUG_ENCODER
    static int32_t value = 0;

    if (h_encoder.value != value)
    {
      value = h_encoder.value;
      printf("Encoder = %ld\r\n", value);
    }
#endif // DEBUG_ENCODER

    //printf( "X_in= %ld    |  Y_in= %ld\r\n", h_dsp.inputs[X_INPUT], h_dsp.inputs[Y_INPUT]);
    printf("Xatt_pot= %ld | Yatt_pot= %ld\r\n\r\n", h_dsp.params[X_ATTV_PARAM], h_dsp.params[Y_ATTV_PARAM]);

    //HAL_Delay(1000);
   

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, GPIO_PIN_SET);

  while (1)
  {
    //printf("Error\r\n, HAL error code: %ld\r\n", HAL_GetError());
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
