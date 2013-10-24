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
-- Filename     dgSaijoApp.c
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
--	1.0	:| C.Boonchuay		:| 20 Oct 2013			:| Read status
-- ----------------------------------------------------------------------
-- ----------------------------------------------------------------------
*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_it.h"
#include "dgSaijoApp.h"
#include "dgHelper.h"
#include BOARD_DES_H

#define SAIJO_RX_BUFF_SIZE	255

bool airStat = false;

unsigned char saijoTxBuff[SAIJO_RX_BUFF_SIZE];
volatile unsigned char saijoBuffTxIdx = 0; 
volatile unsigned char saijoTxBuffLen = 0;


unsigned char saijoRxBuff[SAIJO_RX_BUFF_SIZE];
volatile unsigned char  saijoBuffRxIn = 0;
volatile unsigned char 	saijoBuffRxOut = 0;


const char saijo_buff_on[] = {0x01, 0x06, 0x00, 0x6C, 0x00, 0x01, 0x88, 0x17};
const char saijo_buff_off[] = {0x01, 0x06, 0x00, 0x6C, 0x00, 0x00, 0x49, 0xD7};

unsigned char saijo_buff_fan[FAN_MAX][8] = {
	{0x01, 0x06, 0x00, 0x6E, 0x00, 0x00, 0xE8, 0x17} ,	/* FAN_AUTO */
	{0x01, 0x06, 0x00, 0x6E, 0x00, 0x01, 0x29, 0xD7} ,  /* FAN_LO	*/
	{0x01, 0x06, 0x00, 0x6E, 0x00, 0x02, 0x69, 0xD6} ,  /* FAN_ME	*/
	{0x01, 0x06, 0x00, 0x6E, 0x00, 0x03, 0xA8, 0x16} ,  /* FAN_HI	*/
	{0x01, 0x06, 0x00, 0x6E, 0x00, 0x04, 0xE9, 0xD4} ,  /* FAN_HIHI	*/
	{0x01, 0x06, 0x00, 0x6E, 0x00, 0x05, 0x28, 0x14}  /* FAN_TURBO */
};

/*Private function*/
static void saijo_Send(const unsigned char *buffer, const unsigned char buffLen);
static void SAIJO_WaitSend(void);


static bool currentAirStatus = false;
static uint8_t	currentFanSpeed = 6;

bool isAirPowerON()
{
	return currentAirStatus;
}

uint8_t *getAirFanSpeed()
{
	switch(currentFanSpeed)
	{
	case 0:
		return "Auto   ";
		break;
	
	case 1:
		return "Low    ";
		break;
		
	case 2:
		return "Medium ";
		break;
		
	case 3:
		return "Hi     ";
		break;
	
	case 4:
		return "HiHi   ";
		break;
	case 5:
		return "Turbo  ";
		break;
	
	case 6:
		return "Off    ";
		break;
	}
	return NULL;
}

void dgSaijoInit(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(SAIJO_TX_GPIO_CLK | SAIJO_RX_GPIO_CLK, ENABLE);
	
	/* Enable USART clock */
	SAIJO_UART_CLK_INIT(SAIJO_UART_CLK	, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(SAIJO_TX_GPIO_PORT, SAIJO_TX_SOURCE, SAIJO_TX_AF);
	GPIO_PinAFConfig(SAIJO_RX_GPIO_PORT, SAIJO_RX_SOURCE, SAIJO_RX_AF);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = SAIJO_TX_PIN;
	GPIO_Init(SAIJO_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SAIJO_RX_PIN;
	GPIO_Init(SAIJO_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable the USART OverSampling by 8 */
	USART_OverSampling8Cmd(SAIJO_UART, ENABLE);  
	
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
	USART_InitStructure.USART_BaudRate = 600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(SAIJO_UART, &USART_InitStructure);
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = SAIJO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(SAIJO_UART, ENABLE);
	
//	/* Enabled USART Rx data from zigbee*/
	USART_ITConfig(SAIJO_UART, USART_IT_RXNE, ENABLE);
	dgSaijo_Power_Off();
}

void dgSaijoIncomingHandler(void)
{
	static enum 
	{
		SM_SJ_WAIT_HEAD_1,
		SM_SJ_WAIT_HEAD_2,
		SM_SJ_WAIT_PACKET_COMPLETE,
		SM_SJ_PROCESS_PACKET,
	}sm_SaijoInCommingState;
	
	static uint8_t saijoPacketBuffer[32];
	static uint8_t saijaPacketPtr = 0;
	
	int i;
	
	uint8_t tmpByte;
	
	while(saijoBuffRxOut != saijoBuffRxIn)
	{
		tmpByte = saijoRxBuff[saijoBuffRxOut++];
			
		switch(sm_SaijoInCommingState)
		{
		case SM_SJ_WAIT_HEAD_1:
			if(tmpByte == 0xFE)
			{
				saijaPacketPtr = 0;
				saijoPacketBuffer[saijaPacketPtr++] = tmpByte;
				sm_SaijoInCommingState = SM_SJ_WAIT_HEAD_2;
			}
			break;
		case SM_SJ_WAIT_HEAD_2:
			if(tmpByte == 0xFE)
			{
				saijaPacketPtr = 0;
				saijoPacketBuffer[saijaPacketPtr++] = tmpByte;
				sm_SaijoInCommingState = SM_SJ_WAIT_PACKET_COMPLETE;
			}
			break;
		case SM_SJ_WAIT_PACKET_COMPLETE:
			saijoPacketBuffer[saijaPacketPtr++] = tmpByte;
			if(saijaPacketPtr == 30) /*end of packet, process*/
			{
				if(saijoPacketBuffer[4] & 0x01)
				{
					currentAirStatus = true;
				}
				else
				{
					currentAirStatus = false;
				}
				
				currentFanSpeed = (saijoPacketBuffer[5]>>4) & 0x0F;
				sm_SaijoInCommingState = SM_SJ_WAIT_HEAD_1;
			}
			break;
		}
	}
	
}

void dgSaijo_Power_On(void)
{
	saijo_Send(saijo_buff_on, sizeof(saijo_buff_on));
	airStat = true;
}
void dgSaijo_Power_Off(void)
{
	saijo_Send(saijo_buff_off, sizeof(saijo_buff_off));
	airStat = false;
}
void dgSaijo_FanLev(FAN_LEV lev)
{
	saijo_Send(saijo_buff_fan[lev], 8);
	airStat = false;
}

bool getVar_dgSaijo_Status(void)
{
	return airStat;
}

static void saijo_Send(const unsigned char *buffer, const unsigned char buffLen)
{
	memcpy(saijoTxBuff, buffer, buffLen);
	saijoTxBuffLen = buffLen;
	saijoBuffTxIdx = 0;
	/* Disable the Tx buffer empty interrupt */
	USART_ITConfig(SAIJO_UART, USART_IT_TXE, ENABLE);
}
static void SAIJO_WaitSend(void)
{
	while(saijoBuffTxIdx < saijoTxBuffLen);
}

void SAIJO_IRQHandler(void)
{	
	/* USART in Tramitter mode */
	if (USART_GetITStatus(SAIJO_UART, USART_IT_TXE) == SET)
	{
		if (saijoBuffTxIdx < saijoTxBuffLen)
		{
			/* Send Transaction data */
			USART_SendData(SAIJO_UART, saijoTxBuff[saijoBuffTxIdx++]);
		}
		else
		{
			/* Disable the Tx buffer empty interrupt */
			USART_ITConfig(SAIJO_UART, USART_IT_TXE, DISABLE);
		}
	}
	
//	/* USART in Tramitter mode */
	if (USART_GetITStatus(SAIJO_UART, USART_IT_RXNE) == SET)
	{
		saijoRxBuff[saijoBuffRxIn++] = (uint8_t)USART_ReceiveData(SAIJO_UART);
	}
}
