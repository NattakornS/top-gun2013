/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_it.h"
#include "oledApp.h"
#include "dgSIC4310App.h"
#include "dgHelper.h"
#include "stm32f4xx.h"
#include BOARD_DES_H

#define SIC4310_BUFF_SIZE	64

unsigned char sic4310TxBuff[SIC4310_BUFF_SIZE];
volatile unsigned char sic4310BuffTxIdx = 0; 
volatile unsigned char sic4310TxBuffLen = 0;

unsigned char sic4310RxBuff[SIC4310_BUFF_SIZE];
volatile unsigned char  sic4310BuffRxIn = 0;
volatile unsigned char 	sic4310BuffRxOut = 0;

unsigned int CountPauseDisplay = 0;

void SIC4310_ResetBuffer(void);
static void SIC4310_Send(const unsigned char *, const unsigned char);
static void SIC4310_WaitSend(void);
static void SIC4310_showOLED(void);

void dgSic4310Init (void) {
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(SIC4310_TX_GPIO_CLK | SIC4310_RX_GPIO_CLK, ENABLE);
	
  /* Enable USART clock */
  SIC4310_UART_CLK_INIT(SIC4310_UART_CLK, ENABLE);
	
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(SIC4310_TX_GPIO_PORT, SIC4310_TX_SOURCE, SIC4310_TX_AF);
  GPIO_PinAFConfig(SIC4310_RX_GPIO_PORT, SIC4310_RX_SOURCE, SIC4310_RX_AF);
	
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = SIC4310_TX_PIN;
  GPIO_Init(SIC4310_TX_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = SIC4310_RX_PIN;
  GPIO_Init(SIC4310_RX_GPIO_PORT, &GPIO_InitStructure);
	
  /* Enable the USART OverSampling by 8 */
  USART_OverSampling8Cmd(SIC4310_UART, ENABLE);  
	
  /* USARTx configuration ----------------------------------------------------*/
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(SIC4310_UART, &USART_InitStructure);
	
  /* NVIC configuration */
  /* Configure the Priority Group to 2 bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SIC4310_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  /* Enable USART */
  USART_Cmd(SIC4310_UART, ENABLE);
	
  /* Enabled USART Rx data from 4310*/
  USART_ITConfig(SIC4310_UART, USART_IT_RXNE, ENABLE);
}

void dgSIC4310IncomingHandler(void) {
	uint8_t tmpByte;
	while(sic4310BuffRxOut != sic4310BuffRxIn) {
		tmpByte = sic4310RxBuff[sic4310BuffRxOut++];
		if (tmpByte >= 'a' && tmpByte <= 'z') {
			tmpByte -= 0x20;
		}
		sic4310TxBuff[sic4310TxBuffLen++] = tmpByte; 
	}
	if (sic4310BuffTxIdx != sic4310TxBuffLen) {
	  /*Send data back as upper case*/
	  SIC4310_Send(sic4310TxBuff, sic4310TxBuffLen);
	  SIC4310_WaitSend();
	  
	  /*Update to String OLED */
	  set_FlagNFCUpdated();
	  SIC4310_ResetBuffer();
  }
}

static void SIC4310_Send(const unsigned char *buffer, const unsigned char buffLen)
{
	memcpy(sic4310TxBuff, buffer, buffLen);
	sic4310TxBuffLen = buffLen;
	sic4310BuffTxIdx = 0;
	/* Disable the Tx buffer empty interrupt */
	USART_ITConfig(SIC4310_UART, USART_IT_TXE, ENABLE);
}

static void SIC4310_WaitSend(void)
{
	while(sic4310BuffTxIdx < sic4310TxBuffLen);
}

unsigned char FlagNFCUpdated = 0;

unsigned char IsNFCUpdated(void)
{
	return FlagNFCUpdated;
}
void set_FlagNFCUpdated()
{
	FlagNFCUpdated = 1;
}
void clr_FlagNFCUpdated()
{
	FlagNFCUpdated = 0;
	sic4310RxBuff[0] = '\0';	/*Clear sic4310RxBuff*/
}

//char* getPtr_NFCRxBuff(){
//	return &sic4310RxBuff[0];
//}
		
static void SIC4310_showOLED(void) {
	unsigned char* str = sic4310RxBuff;
	
	//snprintf(str, sic4310BuffRxIn, "%s", sic4310RxBuff);
	
	lcd_Str(0, 0, " TESA TOPGUN2013");
	lcd_Str(0, 1, "----------------");
	lcd_Str(0, 2, "Data From NFC.");
	lcd_Str(0, 3, (char*)str);
	lcd_Str(0, 4,"                ");
	lcd_Str(0, 5,"                ");
	CountPauseDisplay = 1<<24;
}

void SIC4310_IRQHandler(void)
{	
	/* USART in Tramitter mode */
	if (USART_GetITStatus(SIC4310_UART, USART_IT_TXE) == SET)
	{
		if (sic4310BuffTxIdx < sic4310TxBuffLen)
		{
			/* Send Transaction data */
			USART_SendData(SIC4310_UART, sic4310TxBuff[sic4310BuffTxIdx++]);
		}
		else
		{
			/* Disable the Tx buffer empty interrupt */
			USART_ITConfig(SIC4310_UART, USART_IT_TXE, DISABLE);
		}
	}
	
//	/* USART in Tramitter mode */
	if (USART_GetITStatus(SIC4310_UART, USART_IT_RXNE) == SET)
	{
		sic4310RxBuff[sic4310BuffRxIn++] = (uint8_t)USART_ReceiveData(SIC4310_UART);
	}
}

void SIC4310_ResetBuffer() {
  sic4310TxBuffLen = 0;
  sic4310BuffTxIdx = 0;
  sic4310BuffRxOut = 0;
  sic4310BuffRxIn = 0;
}