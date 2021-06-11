#ifndef SYNTH_HPP
#define SYNTH_HPP

#include <stm32f4xx.h>
#include <main.h>
#include <usart.h>
#include <i2c.h>
#include <i2s.h>
#include <dma.h>
#include <tim.h>
#include <dma.h>
#include <adc.h>
#include <gpio.h>


static UART_HandleTypeDef &ext_uart = huart1;
// static UART_HandleTypeDef &midi_uart = huart3;
static I2C_HandleTypeDef &i2c = hi2c1;


#endif
