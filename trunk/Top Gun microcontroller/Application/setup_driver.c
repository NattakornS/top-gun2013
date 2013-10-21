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
//  //USE USART4 Set up for RS-485
//  /*USART Interrupt*/
//  /* Set interrupt: NVIC_Setup */
//  NVIC_InitTypeDef NVIC_InitStruct;
//  //ENABLE USART4 Interruper
//  NVIC_InitStruct.NVIC_IRQChannel = UART4_IRQn;
//  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
//  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStruct.NVIC_IRQChannel = ENABLE;
//  NVIC_Init(&NVIC_InitStruct);

  //Set Up USART
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
  /*
    Use Port A Pin PA0 to Tx
    Use Port A Pin PA1 to Rx
  */
  /* set GPIO init structure parameters values */
  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig( GPIOA, GPIO_PinSource1, GPIO_AF_UART4);
  
  
  /* USART_InitStruct members default value */
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No ;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
  USART_Init(UART4, &USART_InitStruct);
  
  //Enable USART2
  USART_Cmd(UART4, ENABLE);
}

//void NFC_Setup(void)
//{
//  /*
//      NFC_Setup - USE USART6 
//        PC6 - NFC_TX
//        PC7 - NFC_RX
//  */
//  //Set Up USART
//  GPIO_InitTypeDef GPIO_InitStruct;
//  USART_InitTypeDef USART_InitStruct;
//	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
//	
//  /*
//    Use Port C Pin PC6 - NFC_TX
//    Use Port C Pin PC7 - NFC_RX
//  */
//  /* set GPIO init structure parameters values */
//  GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7;
//  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOC, &GPIO_InitStruct);
//  
//  /* Connect PXx to USARTx_Tx*/
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
//  /* Connect PXx to USARTx_Rx*/
//  GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
//  
//  
//  /* USART_InitStruct members default value */
//  USART_InitStruct.USART_BaudRate = 115200;
//  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
//  USART_InitStruct.USART_StopBits = USART_StopBits_1;
//  USART_InitStruct.USART_Parity = USART_Parity_No;
//  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
//  USART_Init(USART6, &USART_InitStruct);
//
//  /* Set Interrupt Mode*/
//  //ENABLE the USART Receive Interrupt
//  //USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
//  //DISABLE the USART Transmit and Receive Interrupt
//  //USART_ITConfig(USART6, USART_IT_TXE, DISABLE);
//  
//  //Enable USART6
//  USART_Cmd(USART6, ENABLE);
//}

