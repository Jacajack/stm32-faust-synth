/**
  ******************************************************************************
  * @file    BSP/Src/main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32469I BSP Drivers
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stlogo.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
  TOUCHSCREEN_DEMO1_INDEX    = 0,
  //TOUCHSCREEN_DEMO2_INDEX    = 1,
  //TOUCHSCREEN_DEMO3_INDEX    = 2,
  //LCD_DEMO_INDEX             = 3,
  //AUDIO_RECORD_INDEX         = 4,
  AUDIO_PLAY_INDEX           = 1,
  //SD_DEMO_INDEX              = 6,
  //LCD_LOG_DEMO_INDEX         = 7,
  //SDRAM_DEMO_INDEX           = 8,
  //SDRAM_DMA_DEMO_INDEX       = 9,
  //SRAM_DEMO_INDEX            = 10,
  //EEPROM_DEMO_INDEX          = 11,
  //QSPI_DEMO_INDEX            = 12,
  //ANIMATION_DEMO_INDEX       = 13,
  MAX_DEMO_INDEX             = 2

} BSP_DemoIndexTypedef;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t DemoIndex = 0;
uint8_t CurrDemoIndex = 0; /* Current executing demo index */
uint8_t NbLoop = 1;
volatile uint8_t exti2_received = 0;
volatile uint8_t exti5_received = 0;

LCD_OrientationTypeDef lcd_display_orientation = LCD_ORIENTATION_INVALID;

/* Global extern variables ---------------------------------------------------*/
uint8_t mfx_toggle_led = 0;


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Display_DemoDescription(void);

BSP_DemoTypedef  BSP_examples[]=
{
  {Touchscreen_demo1, "TOUCHSCREEN DEMO 1", 0},
  #if (TS_MULTI_TOUCH_SUPPORTED == 1)
  //{Touchscreen_demo2, "TOUCHSCREEN DEMO 2", 1},
  #endif
  //{LCD_demo, "LCD", 0},
  {AudioPlay_demo, "AUDIO PLAY", 0},
  //{SD_demo, "mSD", 0},
  //{Log_demo, "LCD LOG", 0},
  //{SDRAM_demo, "SDRAM", 0},
  //{SDRAM_DMA_demo, "SDRAM DMA", 0},
  //{EEPROM_demo, "EEPROM", 0},
  //{QSPI_demo, "QSPI", 0},
  //{Animation_demo, "ANIMATION", 0}
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint8_t  lcd_status = LCD_OK;

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();

  /* Configure the clocks to nominal values */
  SystemClock_Config();

  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);

  /* Configure the User Button in GPIO Mode */
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
  
  /* Initialize DSI LCD */
  lcd_status = BSP_LCD_Init(); 
  BSP_TEST_APPLI_ASSERT(lcd_status != LCD_OK);

  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);   
  BSP_LCD_SelectLayer(0);  
  
  if(BSP_LCD_GetXSize() > BSP_LCD_GetYSize())
  {
    lcd_display_orientation = LCD_ORIENTATION_LANDSCAPE;
  }
  else
  {
    lcd_display_orientation = LCD_ORIENTATION_PORTRAIT;
  }


  Display_DemoDescription();


  /* Wait For User inputs */
  while (1)
  {
    if ( mfx_toggle_led == 1)
    {
      BSP_LED_Toggle(LED1);
      BSP_LED_Toggle(LED2);
      BSP_LED_Toggle(LED3);
      BSP_LED_Toggle(LED4);
      mfx_toggle_led = 0;
    }

    if(BSP_PB_GetState(BUTTON_USER) == PB_RESET)
    {
      HAL_Delay(10);
      while (BSP_PB_GetState(BUTTON_USER) == PB_RESET) { ; }

      /* Current executing demo index */
      CurrDemoIndex = DemoIndex;

      BSP_examples[DemoIndex++].DemoFunc();

      if(DemoIndex >= COUNT_OF_EXAMPLE(BSP_examples))
      {
        /* Increment number of loops  */
        NbLoop++;
        DemoIndex = 0;
      }

      Display_DemoDescription();

    } /* of if(BSP_PB_GetState(BUTTON_USER) == PB_RESET) */

  } /* of while (1) */
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 6
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef  hal_status = HAL_OK;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
#if defined(USE_STM32469I_DISCO_REVA)
  RCC_OscInitStruct.PLL.PLLM = 25;
#else
  RCC_OscInitStruct.PLL.PLLM = 8;
#endif /* USE_STM32469I_DISCO_REVA */
  RCC_OscInitStruct.PLL.PLLN       = 360;
  RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2; /* to have ck_pll = 360 MHz / 2 = 180 MHz */
  RCC_OscInitStruct.PLL.PLLQ       = 7;
  RCC_OscInitStruct.PLL.PLLR       = 6; /* to have ck_plllcd = 60 MHz : replace DPHY PLL clock when this PLL is Off in ULPM mode */
  hal_status = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  BSP_TEST_APPLI_ASSERT(hal_status != HAL_OK);

  /* Activate the Over-Drive mode */
  hal_status = HAL_PWREx_EnableOverDrive();
  BSP_TEST_APPLI_ASSERT(hal_status != HAL_OK);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType =  (RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_HCLK   |
                                 RCC_CLOCKTYPE_PCLK1  |
                                 RCC_CLOCKTYPE_PCLK2);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

  /* Set nominal timing configurations for STM32F469xx */
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1; /* f(hclk) = 180 MHz / 1 = 180 MHz        */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;   /* f(pclk1) = f(hclk)/4  = 180/4 = 45 MHz */
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;   /* f(pclk2) = f(hclk)/2  = 90 MHz         */

  /* Set Flash latency parameters (wait states) depending on CPU clock = ck_sys = 180 MHz */
  hal_status = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  BSP_TEST_APPLI_ASSERT(hal_status != HAL_OK);
}

/**
  * @brief  Display main demo messages.
  * @param  None
  * @retval None
  */
static void Display_DemoDescription(void)
{
  uint8_t desc[50];

  BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

  /* Clear the LCD */
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);

  /* Display LCD messages */
  BSP_LCD_DisplayStringAt(0, 30, (uint8_t *)"STM32F469I_DISCO BSP", CENTER_MODE);
  BSP_LCD_DisplayStringAt(0, 55, (uint8_t *)"Drivers examples", CENTER_MODE);

  /* Draw Bitmap */
  BSP_LCD_DrawBitmap((BSP_LCD_GetXSize() - 80)/2, 125, (uint8_t *)stlogo);

  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()- 40, (uint8_t *)"Copyright (c) STMicroelectronics 2016", CENTER_MODE);

  BSP_LCD_SetFont(&Font16);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(0, BSP_LCD_GetYSize()/2 + 15, BSP_LCD_GetXSize(), 60);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 + 30, (uint8_t *)"Press User Button to start :", CENTER_MODE);
  sprintf((char *)desc,"%s example", BSP_examples[DemoIndex].DemoName);
  BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()/2 + 45, (uint8_t *)desc, CENTER_MODE);
}

/**
  * @brief  Check for user input.
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if(BSP_PB_GetState(BUTTON_USER) == PB_RESET)
  {
    HAL_Delay(10);
    while (BSP_PB_GetState(BUTTON_USER) == PB_RESET);
    return 1 ;
  }
  return 0;
}

/**
  * @brief  Toggle Leds.
  * @param  None
  * @retval None
  */
void Toggle_Leds(void)
{
  static uint32_t ticks = 0;

  if (ticks++ > 1000)
  {
    mfx_toggle_led = 1;
    ticks = 0;
  }
}

/**
  * @brief EXTI line detection callbacks.
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static uint32_t debounce_time = 0;

  if (GPIO_Pin == USER_BUTTON_PIN)
  {
    /* Prevent debounce effect for user key */
    if ((HAL_GetTick() - debounce_time) > 50)
    {
      debounce_time = HAL_GetTick();
    }
    /*function to be completed by else {} */
  }


  if (GPIO_Pin == TS_INT_PIN)
  {
    /* Communication with TS is done via I2C. 
    Often the sw requires ISRs (interrupt service routines) to be quick while communication 
    with I2C can be considered relatively long (depending on SW requirements). 
    Considering that the TS feature don�t need immediate reaction, 
	it is suggested to use polling mode instead of EXTI mode, 
    in order to avoid blocking I2C communication on interrupt service routines */

    /* Here an example of implementation is proposed which is a mix between pooling and exit mode:
    On ISR a flag is set (exti5_received), the main loop polls on the flag rather then polling the TS;
    Mcu communicates with TS only when the flag has been set by ISR. This is just an example: 
    the users should choose they strategy depending on their application needs.*/
    exti5_received = 1;
  }

  if (GPIO_Pin == SD_DETECT_PIN)
  {
    /* Communication with SD is done via I2C. 
    Often the sw requires ISRs (interrupt service routines) to be quick while communication 
    with I2C can be considered relatively long (depending on SW requirements). 
    Considering that the SD feature don�t need immediate reaction, 
	it is suggested to use polling mode instead of EXTI mode, 
    in order to avoid blocking I2C communication on interrupt service routines */

    /* Here an example of implementation is proposed which is a mix between pooling and exit mode:
    On ISR a flag is set (exti2_received), the main loop polls on the flag rather then polling the sd;
    Mcu communicates with sd only when the flag has been set by ISR. This is just an example: 
    the users should choose they strategy depending on their application needs.*/
    exti2_received = 1;
  }
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */
