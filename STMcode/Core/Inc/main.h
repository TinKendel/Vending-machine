/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Data.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void start_ADC();
void automat_Capacity_Lights_Control_Start();
void automat_Capacity_Lights_Control_Stop();
void process_Transmited_Data();
void turn_Off_Automat_Light();
void turn_On_Automat_Light();


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_RS_Pin GPIO_PIN_7
#define LCD_RS_GPIO_Port GPIOE
#define LCD_RW_Pin GPIO_PIN_10
#define LCD_RW_GPIO_Port GPIOE
#define LCD_ENA_Pin GPIO_PIN_11
#define LCD_ENA_GPIO_Port GPIOE
#define LCD_DB4_Pin GPIO_PIN_12
#define LCD_DB4_GPIO_Port GPIOE
#define LCD_DB5_Pin GPIO_PIN_13
#define LCD_DB5_GPIO_Port GPIOE
#define LCD_DB6_Pin GPIO_PIN_14
#define LCD_DB6_GPIO_Port GPIOE
#define LCD_DB7_Pin GPIO_PIN_15
#define LCD_DB7_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
