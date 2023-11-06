/*
 * test_dma_dac.c
 * Used application note:
 * https://www.st.com/resource/en/application_note/an3126-audio-and-waveform-generation-using-the-dac-in-stm32-products-stmicroelectronics.pdf
 *
 *  Created on: Nov 2, 2023
 *      Author: papereir
 */
#include <math.h>

#include <test_dma_dac.h>
#include <math.h>

#define TWELVEBMAX 0xFFF

#define   OUT_FREQ          600                                 // Output waveform frequency
#define   SINE_RES          128                                  // Waveform resolution
#define   CNT_FREQ          16000000      						//APB1 clock config is 90 MHz                       // TIM6 counter clock (prescaled APB1)
#define   TIM_PERIOD        ((CNT_FREQ)/((SINE_RES)*(OUT_FREQ))) // Autoreload reg value

static uint16_t sine12bit[SINE_RES] = { 2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
        2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
        3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
        4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
        3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
        3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
        2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
        1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
        610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
        141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
        129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
        577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
        1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 };

const uint8_t aEscalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

static DAC_ChannelConfTypeDef sConfig;

DAC_Test_RV_TypeDef generate_triangle_wave(TIM_HandleTypeDef* tim_handler, DAC_HandleTypeDef* dac_handler)
{
  HAL_DAC_DeInit(dac_handler);

  HAL_TIM_Base_Start(tim_handler);

  /*##-1- Initialize the DAC peripheral ######################################*/
  if (HAL_DAC_Init(dac_handler) != HAL_OK)
  {
	/* Initialization Error */
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }

  /*##-2- DAC channel1 Configuration #########################################*/
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

  if (HAL_DAC_ConfigChannel(dac_handler, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    /* Channel configuration Error */
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }

  /*##-3- DAC channel1 Triangle Wave generation configuration ################*/
  if (HAL_DACEx_TriangleWaveGenerate(dac_handler, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_1023) != HAL_OK)
  {
    /* Triangle wave generation Error */
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }

  /*##-4- Enable DAC Channel1 ################################################*/
  if (HAL_DAC_Start(dac_handler, DAC_CHANNEL_1) != HAL_OK)
  {
	/* Start Error */
	return DAC_TEST_RV_ERR;
	//Error_Handler();
  }

  /*##-5- Set DAC channel1 DHR12RD register ################################################*/
  if (HAL_DAC_SetValue(dac_handler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x100) != HAL_OK)
  {
	/* Setting value Error */
	return DAC_TEST_RV_ERR;
	// Error_Handler();
  }

  return DAC_TEST_RV_OK;
}

DAC_Test_RV_TypeDef generate_escalator_wave(TIM_HandleTypeDef* tim_handler, DAC_HandleTypeDef* dac_handler, DMA_HandleTypeDef* dma_handler)
{

  HAL_DAC_DeInit(dac_handler);

  HAL_TIM_Base_Start(tim_handler);

  /*##-1- Initialize the DAC peripheral ######################################*/
  if (HAL_DAC_Init(dac_handler) != HAL_OK)
  {
	/* Initialization Error */
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }

  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

  if (HAL_DAC_ConfigChannel(dac_handler, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
	/* Channel configuration Error */
	return DAC_TEST_RV_ERR;
	// Error_Handler();
  }

  /*##-2- Enable DAC selected channel and associated DMA #############################*/
  if (HAL_DAC_Start_DMA(dac_handler, DAC_CHANNEL_1, (uint32_t *)aEscalator8bit, 6, DAC_ALIGN_8B_R) != HAL_OK)
  {
	/* Start DMA Error */
	return DAC_TEST_RV_ERR;
	// Error_Handler();
  }
  return DAC_TEST_RV_OK;
}


DAC_Test_RV_TypeDef generate_sine_wave(TIM_HandleTypeDef* tim_handler, DAC_HandleTypeDef* dac_handler, DMA_HandleTypeDef* dma_handler)
{
  HAL_DAC_DeInit(dac_handler);
  HAL_TIM_Base_DeInit(tim_handler);
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  // TODO: Change ARR value to select frequency.
  // APB1 clock is 90, 000, 000 Hz
  // get to 900 Hz?
  // Trigger frequency = Fclk / ((PSC + 1) * (ARR + 1))
  // OutWaveFrequency = Trigger Frequency / N (number samples)


  tim_handler->Instance = TIM6;
  tim_handler->Init.Prescaler = 0;
  tim_handler->Init.CounterMode = TIM_COUNTERMODE_UP;
  tim_handler->Init.Period = TIM_PERIOD; // ARR + 1 = 65536, trigger freq = 1373 Hz
  tim_handler->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(tim_handler) != HAL_OK)
  {
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(tim_handler, &sMasterConfig) != HAL_OK)
  {
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }

  HAL_TIM_Base_Start(tim_handler);

  /*##-1- Initialize the DAC peripheral ######################################*/
  if (HAL_DAC_Init(dac_handler) != HAL_OK)
  {
	/* Initialization Error */
		return DAC_TEST_RV_ERR;
		// Error_Handler();
  }

  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

  if (HAL_DAC_ConfigChannel(dac_handler, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
	/* Channel configuration Error */
	return DAC_TEST_RV_ERR;
	// Error_Handler();
  }

  /*##-2- Enable DAC selected channel and associated DMA #############################*/
  if (HAL_DAC_Start_DMA(dac_handler, DAC_CHANNEL_1, (uint32_t *)sine12bit, SINE_RES, DAC_ALIGN_12B_R) != HAL_OK) // TODO: Change to sine LUT
  {
	/* Start DMA Error */
	return DAC_TEST_RV_ERR;
	// Error_Handler();
  }

  return DAC_TEST_RV_OK;
}
