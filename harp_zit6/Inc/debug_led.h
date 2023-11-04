#ifndef __DEBUG_LED_H
#define __DEBUG_LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#define LEDn 2

/* 2 Leds are connected to MCU directly on PA8, PA9 (Yellow, Red) */
#define LED1_GPIO_PORT                   ((GPIO_TypeDef*)GPIOA)
#define LED2_GPIO_PORT                   ((GPIO_TypeDef*)GPIOA)

#define LED1_PIN                         ((uint32_t)GPIO_PIN_8)
#define LED2_PIN                         ((uint32_t)GPIO_PIN_9)


typedef enum
{
 LED1 = 0,
 LED_RED = LED1,
 LED2 = 1,
 LED_YELLOW = LED2,
} Led_TypeDef;

/* Exported functions prototypes ---------------------------------------------*/

void             Debug_LED_On(Led_TypeDef Led);
void             Debug_LED_Off(Led_TypeDef Led);
void             Debug_LED_Toggle(Led_TypeDef Led);


#ifdef __cplusplus
}
#endif // cpp

#endif //__DEBUG_LED_H
