/*
PROJECT : TOP GUN Rally 2013 @WU
Microcontroller : STM32F407VG
File : setup_driver.c
*/
/*Note *************************************************************************
      RS485 - USE USART 4 
          PA0 - TX
          PA1 - RX
          PA3 (GPIO3) - RS 485 DT
      OLED - USE I2C (3 wire) (Libraly : oledApp.c)
          PB7 - I2C1_SDA  -> OLED_SCA
          PB6 - I2C1_SCL  -> OLED_SCL
          PD6 - I2C_Reset -> OLED_RESET
      NFC - USE USART6
          PC6 - USART6_TX
          PC7 - USART6_RX
      
*******************************************************************************/
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
// Created Library
#include "setup_driver.h"
//******************************************************************************
// Create Function *************************************************************
void RS485_Setup(void);
void NFC_Setup(void);
//******************************************************************************
void RS485_Setup(void)
{
  //USE USART1 Set up for RS-485

  //Set Up USART
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
  /*
    Use Port B Pin PB6 to Tx
    Use Port B Pin PB7 to Rx
  */
  /* set GPIO init structure parameters values */
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_6;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig( GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
  
  
  /* USART_InitStruct members default value */
  USART_InitStruct.USART_BaudRate = 19200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_Init(USART1, &USART_InitStruct);
  
  /*USART Interrupt*/
  /* Set interrupt: NVIC_Setup */
  NVIC_InitTypeDef NVIC_InitStruct;
  //ENABLE USART4 Interruper
  NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannel = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
  
  //Enable USART1
  USART_Cmd(USART1, ENABLE);
}

void NFC_Setup(void)
{
  *
      NFC_Setup - USE USART6 
        PC6 - NFC_TX
        PC7 - NFC_RX
  */
  //Set Up USART
  //USE USART6 Set up for NFC
  
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
  /*
    Use Port C Pin PC6 - NFC_TX
    Use Port C Pin PC7 - NFC_RX
  */
  /* set GPIO init structure parameters values */
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  
  
  /* USART_InitStruct members default value */
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_Init(USART6, &USART_InitStruct);
  
   /*USART Interrupt*/
  /* Set interrupt: NVIC_Setup */
  NVIC_InitTypeDef NVIC_InitStruct;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  //ENABLE USART4 Interruper
  NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Set Interrupt Mode*/
  //ENABLE the USART Receive Interrupt
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

  //Enable USART6
  USART_Cmd(USART6, ENABLE);
}

