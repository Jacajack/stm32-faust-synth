#include "test_debug_led.h"

void run_debug_led_red(void)
{
	Debug_LED_On(LED_RED);
	HAL_Delay(1000);
	Debug_LED_Off(LED_RED);
	HAL_Delay(1000);
}

void run_debug_led_both(void)
{
	Debug_LED_On(LED_RED);
	HAL_Delay(1000);
	Debug_LED_On(LED_YELLOW);
	HAL_Delay(1000);
	Debug_LED_Off(LED_RED);
	HAL_Delay(1000);
	Debug_LED_Off(LED_YELLOW);
	HAL_Delay(1000);
}
