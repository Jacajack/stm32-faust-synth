/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SystemClock_Config(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_8_Pin GPIO_PIN_13
#define GPIO_8_GPIO_Port GPIOC
#define MUX_A_Pin GPIO_PIN_4
#define MUX_A_GPIO_Port GPIOC
#define MUX_B_Pin GPIO_PIN_5
#define MUX_B_GPIO_Port GPIOC
#define MUX_C_Pin GPIO_PIN_0
#define MUX_C_GPIO_Port GPIOB
#define GPIO_1_Pin GPIO_PIN_6
#define GPIO_1_GPIO_Port GPIOC
#define GPIO_2_Pin GPIO_PIN_7
#define GPIO_2_GPIO_Port GPIOC
#define GPIO_3_Pin GPIO_PIN_8
#define GPIO_3_GPIO_Port GPIOC
#define GPIO_4_Pin GPIO_PIN_9
#define GPIO_4_GPIO_Port GPIOC
#define GPIO_5_Pin GPIO_PIN_10
#define GPIO_5_GPIO_Port GPIOC
#define GPIO_6_Pin GPIO_PIN_11
#define GPIO_6_GPIO_Port GPIOC
#define GPIO_7_Pin GPIO_PIN_12
#define GPIO_7_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
