/* Includes ------------------------------------------------------------------*/
#include "debug_led.h"

uint32_t GPIO_PIN[LEDn] = {LED1_PIN,
                           LED2_PIN};

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED1_GPIO_PORT,
                                 LED2_GPIO_PORT};

/**
  * @brief  Turns selected LED On.
  * @param  Led: LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED1 / LED_YELLOW
  *            @arg  LED2 / LED_RED
  */
void Debug_LED_On(Led_TypeDef Led)
{
  if (Led <= LEDn)
  {
     HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
  }

}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1 / LED_YELLOW
  *            @arg  LED2 / LED_RED
  */
void Debug_LED_Off(Led_TypeDef Led)
{
  if (Led <= LEDn)
  {
    HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
  }
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED1 / LED_YELLOW
  *            @arg  LED2 / LED_RED
  */
void BSP_LED_Toggle(Led_TypeDef Led)
{
  if (Led <= LEDn)
  {
     HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
  }
}

