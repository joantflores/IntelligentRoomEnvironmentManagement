/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PWM_MAX        999
#define WAKE_UP_MSG    1
#define LIGHT_DOWN_MSG 2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t adcValues[4];
uint8_t presenceDetected = 0;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for WakeUpTask */
osThreadId_t WakeUpTaskHandle;
const osThreadAttr_t WakeUpTask_attributes = {
  .name = "WakeUpTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for EnvControlTask */
osThreadId_t EnvControlTaskHandle;
const osThreadAttr_t EnvControlTask_attributes = {
  .name = "EnvControlTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for PIRTask */
osThreadId_t PIRTaskHandle;
const osThreadAttr_t PIRTask_attributes = {
  .name = "PIRTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for EnvQueue */
osMessageQueueId_t EnvQueueHandle;
const osMessageQueueAttr_t EnvQueue_attributes = {
  .name = "EnvQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void UART_Log(const char* msg)
{
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 100);
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartWakeUpTask(void *argument);
void StartEnvControlTask(void *argument);
void StartPIRTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of EnvQueue */
  EnvQueueHandle = osMessageQueueNew (5, 4, &EnvQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of WakeUpTask */
  WakeUpTaskHandle = osThreadNew(StartWakeUpTask, NULL, &WakeUpTask_attributes);

  /* creation of EnvControlTask */
  EnvControlTaskHandle = osThreadNew(StartEnvControlTask, NULL, &EnvControlTask_attributes);

  /* creation of PIRTask */
  PIRTaskHandle = osThreadNew(StartPIRTask, NULL, &PIRTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartWakeUpTask */
/**
* @brief Function implementing the WakeUpTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWakeUpTask */
void StartWakeUpTask(void *argument)
{
	/* USER CODE BEGIN StartWakeUpTask */
	for(;;)
	{
	    osDelay(1000);
	}
	/* USER CODE END StartWakeUpTask */
}

/* USER CODE BEGIN Header_StartEnvControlTask */
/**
* @brief Function implementing the EnvControlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEnvControlTask */
void StartEnvControlTask(void *argument)
{
  /* USER CODE BEGIN StartEnvControlTask */
	uint32_t msg;
	uint32_t targetBrightness;
	char uartBuf[64];

	for(;;)
	{
	    if(osMessageQueueGet(EnvQueueHandle, &msg, NULL, osWaitForever) == osOK)
	    {
	    	// Leer los 4 potenciómetros
	    	HAL_ADC_Stop(&hadc1);
	    	HAL_ADC_Start(&hadc1);
	    	HAL_ADC_PollForConversion(&hadc1, 100);
	    	adcValues[0] = HAL_ADC_GetValue(&hadc1);
	    	HAL_ADC_PollForConversion(&hadc1, 100);
	    	adcValues[1] = HAL_ADC_GetValue(&hadc1);
	    	HAL_ADC_PollForConversion(&hadc1, 100);
	    	adcValues[2] = HAL_ADC_GetValue(&hadc1);
	    	HAL_ADC_PollForConversion(&hadc1, 100);
	    	adcValues[3] = HAL_ADC_GetValue(&hadc1);

	        if(msg == WAKE_UP_MSG)
	        {
	            UART_Log("[WAKE_UP] Rampa iniciando...\r\n");

	            for(uint32_t step = 0; step <= 100; step++)
	            {
	                // LED 1 - TIM1 CH3 (D3)
	                targetBrightness = (adcValues[0] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,
	                    (targetBrightness * step) / 100);

	                // LED 2 - TIM1 CH2 (D5)
	                targetBrightness = (adcValues[1] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,
	                    (targetBrightness * step) / 100);

	                // LED 3 - TIM1 CH1 (D6)
	                targetBrightness = (adcValues[2] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
	                    (targetBrightness * step) / 100);

	                // LED 4 - TIM4 CH4 (D9)
	                targetBrightness = (adcValues[3] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4,
	                    (targetBrightness * step) / 100);

	                // Enviar log UART cada 10 pasos
	                if(step % 10 == 0)
	                {
	                    snprintf(uartBuf, sizeof(uartBuf),
	                        "[DIMMER] Nivel: %lu%% | POT1:%lu POT2:%lu POT3:%lu POT4:%lu\r\n",
	                        step,
	                        (adcValues[0] * 100) / 4095,
	                        (adcValues[1] * 100) / 4095,
	                        (adcValues[2] * 100) / 4095,
	                        (adcValues[3] * 100) / 4095);
	                    UART_Log(uartBuf);
	                }

	                osDelay(50);
	            }
	            UART_Log("[WAKE_UP] Rampa completada.\r\n");
	        }
	        else if(msg == LIGHT_DOWN_MSG)
	        {
	            UART_Log("[LIGHT_DOWN] Bajando brillo...\r\n");

	            for(int32_t step = 100; step >= 0; step--)
	            {
	                targetBrightness = (adcValues[0] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,
	                    (targetBrightness * step) / 100);

	                targetBrightness = (adcValues[1] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,
	                    (targetBrightness * step) / 100);

	                targetBrightness = (adcValues[2] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,
	                    (targetBrightness * step) / 100);

	                targetBrightness = (adcValues[3] * PWM_MAX) / 4095;
	                __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4,
	                    (targetBrightness * step) / 100);

	                if(step % 10 == 0)
	                {
	                    snprintf(uartBuf, sizeof(uartBuf),
	                        "[DIMMER] Nivel: %ld%%\r\n", step);
	                    UART_Log(uartBuf);
	                }

	                osDelay(50);
	            }
	            UART_Log("[LIGHT_DOWN] Brillo apagado.\r\n");
	        }
	    }
	}
  /* USER CODE END StartEnvControlTask */
}

/* USER CODE BEGIN Header_StartPIRTask */
/**
* @brief Function implementing the PIRTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartPIRTask */
void StartPIRTask(void *argument)
{
	/* USER CODE BEGIN StartPIRTask */
	for(;;)
	{
	    osDelay(1000);
	}
	/* USER CODE END StartPIRTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_9)
    {
        uint32_t msg;
        if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_9) == GPIO_PIN_SET)
        {
            if(presenceDetected == 0)
            {
                presenceDetected = 1;
                msg = WAKE_UP_MSG;
                UART_Log("[PIR] Presencia detectada - Encendiendo luces\r\n");
                osMessageQueuePut(EnvQueueHandle, &msg, 0, 0);
            }
        }
        else
        {
            if(presenceDetected == 1)
            {
                presenceDetected = 0;
                msg = LIGHT_DOWN_MSG;
                UART_Log("[PIR] Sin presencia - Apagando luces\r\n");
                osMessageQueuePut(EnvQueueHandle, &msg, 0, 0);
            }
        }
    }
}
/* USER CODE END Application */

