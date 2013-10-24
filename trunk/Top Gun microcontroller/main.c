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
char NFC_InputStore[11];                                                                 //Variable for Store Data form NFC
char NFC_Upper[11];
uint8_t i=0, j=0,x=0;                                                                      //Varable for Array
void delay(void);
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
    if(i == 10)
    {
      lcd_Str(1,2,NFC_InputStore);
      for(x=0;x<11;x++)
      {
          USART_SendData(USART6,NFC_Upper[x]);
          while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
      }
    }
  }
  
}

// Interrupt Sevices Routine ***************************************************
void USART6_IRQHandler(void)
{
  NFC_input = USART_ReceiveData(USART6);
  USART_ClearITPendingBit(USART6, USART_IT_RXNE);
  NFC_InputStore[i] = NFC_input;
  NFC_Upper[i] = NFC_input - 0x20;
  i++;
  i = i% 11;
}
//******************************************************************************
void delay(void)
{
  unsigned char i;
  for(i=0;i<60000;i++);
}


int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART6, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET)
  {}
  return ch;
}
