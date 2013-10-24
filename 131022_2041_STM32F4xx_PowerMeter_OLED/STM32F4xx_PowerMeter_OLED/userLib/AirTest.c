/*  */ 

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_it.h"

#include "sx1App.h"
//#include "oledApp.h"
#include BOARD_DES_H


/**
* @brief  Configures the USART Peripheral.
* @param  None
* @retval None
*/
static void Air_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd( Air_TX_GPIO_CLK | Air_RX_GPIO_CLK, ENABLE);
	
	/* Enable USART clock */
	RS485_UART_CLK_INIT(Air_UART_CLK, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(Air_TX_GPIO_PORT, Air_TX_SOURCE, Air_TX_AF);
	GPIO_PinAFConfig(Air_RX_GPIO_PORT, Air_RX_SOURCE, Air_RX_AF);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = RS485_TX_PIN;
	GPIO_Init(RS485_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = Air_RX_PIN;
	GPIO_Init(Air_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable the USART OverSampling by 8 */
	USART_OverSampling8Cmd(Air_UART, ENABLE);  
	
	/* USARTx configuration ----------------------------------------------------*/
	/* USARTx configured as follow:
	- BaudRate = 5250000 baud
	- Maximum BaudRate that can be achieved when using the Oversampling by 8
	is: (USART APB Clock / 8) 
	Example: 
	- (USART3 APB1 Clock / 8) = (42 MHz / 8) = 5250000 baud
	- (USART1 APB2 Clock / 8) = (84 MHz / 8) = 10500000 baud
	- Maximum BaudRate that can be achieved when using the Oversampling by 16
	is: (USART APB Clock / 16) 
	Example: (USART3 APB1 Clock / 16) = (42 MHz / 16) = 2625000 baud
	Example: (USART1 APB2 Clock / 16) = (84 MHz / 16) = 5250000 baud
	- Word Length = 8 Bits
	- one Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/ 
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(Air_UART, &USART_InitStructure);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RS485_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(Air_UART, ENABLE);
}




void Air_IRQHandler(void)
{	
	/* USART in Tramitter mode */
	if (USART_GetITStatus(Air_UART, USART_IT_TXE) == SET)
	{
		if (AirBuffTxIdx < AirBuffLen)
		{
			/* Send Transaction data */
			USART_SendData(Air_UART, AirBuff[AirBuffTxIdx++]);
		}
		else
		{
			/* Disable the Tx buffer empty interrupt */
			USART_ITConfig(Air_UART, USART_IT_TXE, DISABLE);
		}
	}
}