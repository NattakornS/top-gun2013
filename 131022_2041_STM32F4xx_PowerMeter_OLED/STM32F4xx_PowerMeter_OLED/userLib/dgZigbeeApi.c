/*
-- ----------------------------------------------------------------------
--
-- 
-- ################             ###############    ########   ########  #######   ###   ########   ###   ###                 
-- #################          #################    #########  ########  ### ####  ###   ########   ####  ###                 
-- ##################        ##################    ###   ###  ###       ###  ##   ###  ###    #    ####  ###                 
-- ###################      ###################    ###   ###  ########  ######    ###  ###         ##### ###                 
-- ####################    ####################    ###   ###  ########   #######  ###  ###  #####  ## ######                 
-- #######     ########    ########                ###   ###  ###           ####  ###  ###  #####  ## ######                 
-- #######      ########   #######                 ###   ###  ###      ###   ###  ###  ###    ###  ##  #####                 
-- #######       #######  ########                 #########  ########  ###  ###  ###   #########  ##   ####                 
-- #######       #######  #######                  ########   ########  #######   ###   ########   ##   ####                 
-- #######       #######  #######                                         ###             ####
-- #######       #######  #######     #########      ######      ####   ######### ############   ###  ###    ###   ###    ###
-- #######       #######  #######     #########     ########     ####   ######### ######### ##  ####  ###   #####   ###  ### 
-- #######       #######  #######     #########    ####   ###   ######     ###    ###       ### ####  ##    #####   ### #### 
-- #######       #######  #######     #########    ###          ######     ###    ###       ### ########   ### ##    ######  
-- #######       #######  #######       #######    ###  #####   ##  ##     ###    ########  ############   ### ###    ####   
-- #######      ########   #######      #######    ###  #####  ###  ###    ###    ########   ##### #####   ##  ###    ####   
-- #######      #######    ########     #######    ###    ###  ########    ###    ###        ####  #####  #########   ###    
-- ####################    ####################    ####   ###  #########   ###    ###        ####  ####   #########   ###    
-- ###################      ###################     ######### ###    ###   ###    #########  ####   ###   ###   ###   ###    
-- ##################        ##################      ######   ###    ###   ###    #########  ####   ###  ###    ####  ###    
-- #################          #################                                                                              
-- ################             ###############                                                                              
--
-- >>>>>>>>>>>>>>>>>>>>>>>>> COPYRIGHT NOTICE <<<<<<<<<<<<<<<<<<<<<<<<<<<
-- ----------------------------------------------------------------------
-- Copyright 2013 (c) Design Gateway Co.,Ltd
--
-- Design Gateway Co.,Ltd owns the sole copyright to this software. 
-- Under international copyright laws you 
-- (1) may not make a copy of this software except for the purposes of 
--     maintaining a single archive copy, 
-- (2) may not derive works herefrom,
-- (3) may not distribute this work to others. These rights are provided 
--     for information clarification, other restrictions of rights may 
--     apply as well.
--
-- This is an unpublished work.
-- ----------------------------------------------------------------------
-- >>>>>>>>>>>>>>>>>>>>>>>>>>>>> Warrantee <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
-- ----------------------------------------------------------------------
--
-- Design Gateway Co.,Ltd MAKES NO WARRANTY OF ANY KIND WITH REGARD TO THE 
-- USE OF THIS SOFTWARE, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT 
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A 
-- PARTICULAR PURPOSE.
--			  
-- --------------------------------------------------------------------------------
-- --------------------------------------------------------------------------------
-- Filename     dgZigbeeApi.c
-- Company      Design Gateway Co.,Ltd
-- Syntax       C
-- Detail		
-- Note        
--
-- ----------------------------------------------------------------------
-- Revision History :
-- ----------------------------------------------------------------------
--	Rev	:| Author         	:| Mod. Date        	:| Detail of Change :
-- ----------------------------------------------------------------------
--	1.0	:| P.Nitiwattananon	:| 15 Sep 2013			:| Initial Release
-- ----------------------------------------------------------------------
-- ----------------------------------------------------------------------
*/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "stm32f4xx_it.h"
#include "dgZigbeeApi.h"
#include "dgSaijoApp.h"
#include "dgHelper.h"


#define ZB_RX_BUFF_SIZE	255


unsigned char zbTxBuff[ZB_RX_BUFF_SIZE];
volatile uint8_t zbBuffTxIdx = 0; 
volatile unsigned char zbTxBuffLen = 0;

unsigned char zbRxBuff[ZB_RX_BUFF_SIZE];
unsigned char zbRxBuff_WrIdx=0;
unsigned char zbRxBuff_RdIdx=0;


static void ZB_Send(unsigned char *buffer, unsigned char buffLen);
static void ZB_WaitSend(void);

/*Rx FIFO*/
static void ZB_PushRxBuff(unsigned char byte);
static bool ZB_GetRxBuff(unsigned char* retByte);


void dgZigbeeApiInit(void)
{

	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(ZB_TX_GPIO_CLK | ZB_RX_GPIO_CLK, ENABLE);
	
	/* Enable USART clock */
	ZB_UART_CLK_INIT(ZB_UART_CLK	, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(ZB_TX_GPIO_PORT, ZB_TX_SOURCE, ZB_TX_AF);
	GPIO_PinAFConfig(ZB_RX_GPIO_PORT, ZB_RX_SOURCE, ZB_RX_AF);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = ZB_TX_PIN;
	GPIO_Init(ZB_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = ZB_RX_PIN;
	GPIO_Init(ZB_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable the USART OverSampling by 8 */
	USART_OverSampling8Cmd(ZB_UART, ENABLE);  
	
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
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(ZB_UART, &USART_InitStructure);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ZB_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(ZB_UART, ENABLE);
	
	/* Enabled USART Rx data from zigbee*/
	USART_ITConfig(ZB_UART, USART_IT_RXNE, ENABLE);
}
#define ZB_RX_STR_SIZE	4096
void dgZigbeeApiIncomingHandler(void)
{
	static unsigned short zbIncomingTick = 0;
	static unsigned short zbStrIdx;
	static unsigned char zbRxStr[ZB_RX_STR_SIZE];
	static enum{
		ZB_INCOMING_IDLE = 0,
		ZB_INCOMING_MON_END,
		ZB_INCOMING_PROC,
		ZB_INCOMING_LEAVE_BUFFER,
	}zb_incoming_st = ZB_INCOMING_IDLE;
	unsigned char rxByte = 0;
	
	switch(zb_incoming_st)
	{
	case ZB_INCOMING_IDLE:
		if(ZB_GetRxBuff(&rxByte))
		{
			if(rxByte == '$')
			{
				zbRxStr[zbStrIdx++] = rxByte;
				if(zbStrIdx >= ZB_RX_STR_SIZE)
				{
					/*Buffer ovf*/
					zb_incoming_st = ZB_INCOMING_LEAVE_BUFFER;
				}
				zb_incoming_st = ZB_INCOMING_MON_END;	
			}
			zbIncomingTick = getVar_Time_Rec_Base();
		}
		else
		{
			if((getVar_Time_Rec_Base()-zbIncomingTick) > 100)	/*Timeout 1second*/
			{
				zb_incoming_st = ZB_INCOMING_LEAVE_BUFFER;
				zbIncomingTick = getVar_Time_Rec_Base();
			}
		}
		break;
	case ZB_INCOMING_MON_END:
		if(ZB_GetRxBuff(&rxByte))
		{
			zbRxStr[zbStrIdx++] = rxByte;
			if(rxByte == ';')
			{
				zbRxStr[zbStrIdx++] = '\0';
				zbRxStr[zbStrIdx++] = '\0';
				zb_incoming_st = ZB_INCOMING_PROC;
			}
			/**/
			zbIncomingTick = getVar_Time_Rec_Base();
		}
		else
		{
			if((getVar_Time_Rec_Base()-zbIncomingTick) > 100)	/*Timeout 1second*/
			{
				zb_incoming_st = ZB_INCOMING_LEAVE_BUFFER;
				zbIncomingTick = getVar_Time_Rec_Base();
			}
		}
		break;
	case ZB_INCOMING_PROC:
		/*Process zbRxStr*/
		{
			char* tmpStr;
			unsigned char tmpByte[255];
			unsigned short tmpLen;
			tmpLen = strlen(zbRxStr);
			
			/* Called DG's proprietary function */
			formatIncomingString(zbRxStr);

			/* Called DG's proprietary function */
			tmpStr = emberGetIncommingArg(zbRxStr, "payload");
			if(tmpStr != NULL){
				
				/* Called DG's proprietary function */
				tmpLen = extractPayload(tmpStr, tmpByte);
				if(tmpLen > 0)
				{
					tmpByte[tmpLen] = '\0';
					
					tmpByte[0];	/*Len*/
					switch(tmpByte[1])	/*endpoint*/
					{
					case 'r':	/* relay */
						if(strcmp(&tmpByte[2], "1_SET_ON") == 0)
						{
							abEval_RelaySet(RELAY1, RELAY_ST_ON);
						}
						else if(strcmp(&tmpByte[2], "1_SET_OFF") == 0)
						{
							abEval_RelaySet(RELAY1, RELAY_ST_OFF);
						}
						else if(strcmp(&tmpByte[2], "2_SET_ON") == 0)
						{
							abEval_RelaySet(RELAY2, RELAY_ST_ON);
						}
						else if(strcmp(&tmpByte[2], "2_SET_OFF") == 0)
						{
							abEval_RelaySet(RELAY2, RELAY_ST_OFF);
						}
						break;
						
					case 'a':	/* air condition */
						if(strcmp(&tmpByte[2], "POWER_ON") == 0)
						{
							dgSaijo_Power_On();
						}
						else if(strcmp(&tmpByte[2],"POWER_OFF") == 0)
						{
							dgSaijo_Power_Off();
						}
						else if(strcmp(&tmpByte[2],"FAN_AUTO") == 0)
						{
							dgSaijo_FanLev(FAN_AUTO);
						}
						else if(strcmp(&tmpByte[2],"FAN_LO") == 0)
						{
							dgSaijo_FanLev(FAN_LO);						
						}
						else if(strcmp(&tmpByte[2],"FAN_ME") == 0)
						{
							dgSaijo_FanLev(FAN_ME);
						}
						else if(strcmp(&tmpByte[2],"FAN_HI") == 0)
						{
							dgSaijo_FanLev(FAN_HI);
						}
						else if(strcmp(&tmpByte[2],"FAN_2HI") == 0)
						{
							dgSaijo_FanLev(FAN_HIHI);
						}
						else if(strcmp(&tmpByte[2],"FAN_TURBO") == 0)
						{                           
							dgSaijo_FanLev(FAN_TURBO);
						}
						break;
					case 0xF0:
						/*AirBean status Endpoint*/
						break;
					default:
						/*Others endpoint*/
						break;
					}
				}
			}
		}
		zb_incoming_st = ZB_INCOMING_LEAVE_BUFFER;
		zbIncomingTick = getVar_Time_Rec_Base();
		break;
	case ZB_INCOMING_LEAVE_BUFFER:
		{
			
			zbStrIdx = 0;
			zb_incoming_st = ZB_INCOMING_IDLE;
		}

	}
	
}
unsigned char tmpStr[255];
void dgZigbeeApiSendString(unsigned short dstId, char* buffer )
{
	/*
	abApi send str 0x0000 "helloworld"\n\r
	*/
#if 1
	sprintf(tmpStr, "abApi send str %d \"%s\"\n\r", dstId, buffer);
	ZB_Send(tmpStr, strlen(tmpStr));
	ZB_WaitSend();
#else
	sprintf(tmpStr, "abApi send str %d \"",dstId);
	ZB_Send(tmpStr, strlen(tmpStr));
	ZB_WaitSend();
	
	ZB_Send(buffer, strlen(buffer));
	ZB_WaitSend();
	
	sprintf(tmpStr, "\"\n\r");
	ZB_Send(tmpStr, strlen(tmpStr));
	ZB_WaitSend();
#endif
}
void dgZigbeeApiSendRaw(unsigned short dstId, unsigned char* buffer, unsigned char bufferLen )
{
	sprintf(tmpStr, "abApi send raw %d {", dstId);
	ZB_Send(tmpStr, strlen(tmpStr));
	ZB_WaitSend();
	
	for(unsigned char i=0;i<bufferLen;i++)
	{
		sprintf(tmpStr, "%02X ", buffer[i]);
		ZB_Send(tmpStr, strlen(tmpStr));
		ZB_WaitSend();
	}
	sprintf(tmpStr, "}\n\r");
	ZB_Send(tmpStr, strlen(tmpStr));
	ZB_WaitSend();
}



static void ZB_Send(unsigned char *buffer, unsigned char buffLen)
{
	memcpy(zbTxBuff, buffer, buffLen);
	zbTxBuffLen = buffLen;
	zbBuffTxIdx = 0;
	/* Disable the Tx buffer empty interrupt */
	USART_ITConfig(ZB_UART, USART_IT_TXE, ENABLE);
}
static void ZB_WaitSend(void)
{
	while(zbBuffTxIdx < zbTxBuffLen);
}

static void ZB_PushRxBuff(unsigned char byte)
{
	zbRxBuff[zbRxBuff_WrIdx] = byte;
	zbRxBuff_WrIdx = (zbRxBuff_WrIdx+1)%ZB_RX_BUFF_SIZE;
}

static bool ZB_GetRxBuff(unsigned char* retByte)
{
	if(zbRxBuff_RdIdx != zbRxBuff_WrIdx)
	{
		*retByte = zbRxBuff[zbRxBuff_RdIdx];
		zbRxBuff_RdIdx = (zbRxBuff_RdIdx + 1)%ZB_RX_BUFF_SIZE;
		return true;
	}
	else
	{
		/* No data in FIFO */
		return false;
	}
}

void ZB_IRQHandler(void)
{	
	/* USART in Tramitter mode */
	if (USART_GetITStatus(ZB_UART, USART_IT_TXE) == SET)
	{
		if (zbBuffTxIdx < zbTxBuffLen)
		{
			/* Send Transaction data */
			USART_SendData(ZB_UART, zbTxBuff[zbBuffTxIdx++]);
		}
		else
		{
			/* Disable the Tx buffer empty interrupt */
			USART_ITConfig(ZB_UART, USART_IT_TXE, DISABLE);
		}
	}
	
	/* USART in Tramitter mode */
	if (USART_GetITStatus(ZB_UART, USART_IT_RXNE) == SET)
	{
		ZB_PushRxBuff(USART_ReceiveData(ZB_UART));
	}
}

