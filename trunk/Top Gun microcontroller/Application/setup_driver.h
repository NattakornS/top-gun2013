/*
PROJECT : TOP GUN Rally 2013 @WU
Microcontroller : STM32F407VG
File : setup_driver.h
*/
// Include Library *************************************************************
// Standard Library
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
// Create Library

//******************************************************************************
//Define Function **************************************************************
void RS485_Setup (void);                                                        // USE USART 4 for set up RS-485
void OLED_Setup (void);                                                         //OLED Set by I2C1
void NFC_Setup(void);                                                           //Function NFC Set Up
