/*
PROJECT : TOP GUN Rally 2013 @WU
Microcontroller : STM32F407VG
File : main.c
*/
// Include Library *************************************************************
// Standard Library
#include "stm32f4xx.h"
#include <stdio.h>
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_crc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "oledApp.h"
// Create Library
#include "setup_driver.h"
//******************************************************************************
// Create Global Variable ******************************************************
uint16_t Data_in;
char NFC_input;
char NFC_InputStore[8];                                                                 //Variable for Store Data form NFC
uint8_t i=0, j=0;                                                                      //Varable for Array
void delay(void);
void NFC_Setup1(void);
//******************************************************************************
// Main Function ***************************************************************
int main()
{
  /* Initinal Function */
  NFC_Setup1();
  RS485_Setup();
  Oled_SSD1305_Init();
  lcd_Str(1, 1,"Hello TESA");
  while(1)
  {
    NFC_InputStore[i] = NFC_input;
    lcd_Str(1,2,NFC_InputStore);
  }
  
}

// Interrupt Sevices Routine ***************************************************
void USART6_IRQHandler(void)
{
  NFC_input = USART_ReceiveData(USART6);
  USART_ClearITPendingBit(USART6, USART_IT_RXNE);
  i++;
  
  
/
}
//******************************************************************************
void NFC_Setup1(void)
{
  /*
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

void delay(void)
{
  unsigned char i;
  for(i=0;i<60000;i++);
}


int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
  return ch;
}
