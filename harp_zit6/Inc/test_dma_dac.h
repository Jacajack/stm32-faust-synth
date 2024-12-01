/*
 * test_triangle_gen.h
 *
 *  Created on: Nov 2, 2023
 *      Author: papereir
 */

#ifndef INC_TEST_DMA_DAC_H_
#define INC_TEST_DMA_DAC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Defines  ------------------------------------------------------------------*/

typedef enum
{
 DAC_TEST_RV_OK = 0,
 DAC_TEST_RV_ERR = 1,
} DAC_Test_RV_TypeDef;

/* Exported functions prototypes ---------------------------------------------*/

DAC_Test_RV_TypeDef generate_triangle_wave(TIM_HandleTypeDef* tim_handler, DAC_HandleTypeDef* dac_handler);
DAC_Test_RV_TypeDef generate_escalator_wave(TIM_HandleTypeDef* tim_handler, DAC_HandleTypeDef* dac_handler, DMA_HandleTypeDef* dma_handler);
DAC_Test_RV_TypeDef generate_sine_wave(TIM_HandleTypeDef* tim_handler, DAC_HandleTypeDef* dac_handler, DMA_HandleTypeDef* dma_handler);


#ifdef __cplusplus
}
#endif // cpp

#endif /* INC_TEST_DMA_DAC_H_ */
