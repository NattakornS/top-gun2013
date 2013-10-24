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
-- Filename     sx1App.c
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
--	1.0	:| C.Boonchuay		:| 20 Oct 2013			:| Auto scan Meter-ID
-- ----------------------------------------------------------------------
-- ----------------------------------------------------------------------
*/



/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_it.h"

#include "sx1App.h"
#include "dgHelper.h"
#include BOARD_DES_H

typedef enum{
	SM_CONNECTING,
	SM_ACK,
	SM_CONNECTED,
	
	SM_READ_METERID,
	SM_READ_METER_VOLT,
	SM_READ_METER_CURRENT,
	SM_READ_METER_WATT,
	
	
	//SM_READ_DATA,
	
	SM_GET_DATA,
	SM_DISCONNECT,
	SM_WAIT,
}SM_METER_STATE;
//SM_State = SM_CONNECTING ;

static uint8_t constMeterID[] = {0,95,157,93,198,197,199,96,94,159,160};

uint8_t aTxBuffer[BUFFERSIZE] ;
uint8_t aRxBuffer [51];

uint8_t pSensorSoftBuffer[51];
uint8_t ACKBuffer[51];
float DataBuffer [9];
int EnergyValue = 0 ;

//int rs485Id = METERID_2704236  ;


//uint8_t rs485Id = METERID  ;


__IO uint8_t ubRxIndex = 0x00;
__IO uint8_t ubTxIndex = 0x00;

static void RS485_USART_Config(void);
static bool CheckACK(void);
static void DecodeEnergyValue(void);
void Make_PowermeterPacket( uint8_t type );
//unsigned char powermeter_meterToRs485( unsigned char *meterId, int id_len );
unsigned short powermeter_calCrc( unsigned char *msg, int msg_len );
unsigned char powermeter_calBcc( unsigned char *msg, int msg_len );

METER_STATUS meterStat = METER_ERR;
static uint8_t correntScanID = 0;
static int currentMeterID = 0;

static uint32_t fullMeterId = 0;
static uint32_t 	readingVoltege;
static uint32_t	readingCurrent;


uint32_t	getMeterId()
{
	return fullMeterId;
}

int getVar_EnergyValue(void)
{
	return EnergyValue;
}

uint32_t getMeterVoltage()
{
	return readingVoltege;
}
uint32_t getMeterCurrent()
{
	return readingCurrent;
}

METER_STATUS getVar_meterStatus(void)
{
	return meterStat;
}
void sx1App_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RS485_DIR_GPIO_CLK, ENABLE);
	
	/* Configure PG6 and PG8 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = RS485_DIR_PIN ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(RS485_DIR_GPIO_PORT, &GPIO_InitStructure);
	
    RS458_DIR_RX();
	/* USART configuration -----------------------------------------------------*/
	RS485_USART_Config(); 
}

void deCodeMeterSerialNumber(void)
{
	uint8_t tmpStr[16];
	tmpStr[0] =  (aRxBuffer[6]&0x0F) + '0';
	tmpStr[1] =  (aRxBuffer[7]&0x0F) + '0';
	tmpStr[2] =  (aRxBuffer[8]&0x0F) + '0';
	tmpStr[3] =  (aRxBuffer[9]&0x0F) + '0';
	tmpStr[4] =  (aRxBuffer[10]&0x0F) + '0';
	tmpStr[5] =  (aRxBuffer[11]&0x0F) + '0';
	tmpStr[6] =  (aRxBuffer[12]&0x0F) + '0';
	
	tmpStr[7] = 0;
	fullMeterId = atoi((char*)&tmpStr[0]);
}

void deCodeMeterVoltage(void)
{
//	uint32_t tmpDword;
	uint8_t tmpStr[16];
	tmpStr[0] =  (aRxBuffer[6]&0x0F) + '0';
	tmpStr[1] =  (aRxBuffer[7]&0x0F) + '0';
	tmpStr[2] =  (aRxBuffer[8]&0x0F) + '0';
	tmpStr[3] =  (aRxBuffer[9]&0x0F) + '0';
	tmpStr[4] =  (aRxBuffer[10]&0x0F) + '0';
//	tmpStr[5] =  (aRxBuffer[11]&0x0F) + '0';
//	tmpStr[6] =  (aRxBuffer[12]&0x0F) + '0';
	
	tmpStr[5] = 0;
	readingVoltege = atoi((char*)&tmpStr[0]);
	//readingVoltege = (float)tmpDword / 100.00;
}

void deCodeMeterCurrent(void)
{
//	uint32_t tmpDword;
	uint8_t tmpStr[16];
	tmpStr[0] =  (aRxBuffer[6]&0x0F) + '0';
	tmpStr[1] =  (aRxBuffer[7]&0x0F) + '0';
	tmpStr[2] =  (aRxBuffer[8]&0x0F) + '0';
	tmpStr[3] =  (aRxBuffer[9]&0x0F) + '0';
	tmpStr[4] =  (aRxBuffer[10]&0x0F) + '0';
//	tmpStr[5] =  (aRxBuffer[11]&0x0F) + '0';
//	tmpStr[6] =  (aRxBuffer[12]&0x0F) + '0';
	
	tmpStr[5] = 0;
	readingCurrent = atoi((char*)&tmpStr[0]);
	//readingCurrent = (float)tmpDword / 10.00;
}

void sx1App_Process(void)
{
	static uint16_t smWaitTick;
	static SM_METER_STATE SM_State = SM_CONNECTING ;
	static uint8_t lastReadingCommand;
	
	switch(SM_State)
	{
	case SM_CONNECTING:
//		/*check meter is connect, move to reading meter data*/
//		if(meterStat == METER_ERR)
//		{
			/*currently is not detect meter scan*/
			Make_PowermeterPacket(0);
			ubTxIndex = 0;
			USART_ITConfig(RS485_UART, USART_IT_RXNE, DISABLE);
			/* Enable the Tx buffer empty interrupt */
			RS458_DIR_TX();
			USART_ITConfig(RS485_UART, USART_IT_TXE, ENABLE); // State CONNECT
			SM_State =  SM_ACK ;   
			smWaitTick = getVar_Time_Rec_Base();
//		}
//		else
//		{
//			/*last meter  detect, start reading data*/
//			SM_State =  SM_CONNECTED ;
//		}
		break;
		
	case SM_ACK:
		if( ubRxIndex == 51  ) 
		{
			if(CheckACK())
			{
				smWaitTick = getVar_Time_Rec_Base();
				SM_State =  SM_CONNECTED ;
			}
			else
			{
				
				aRxBuffer[0] = 0;
				aRxBuffer[1] = 0;
				SM_State =  SM_CONNECTING;
				meterStat = METER_ERR;
			}
		}  
		
		/*Wait meter ack is ok*/
		if((getVar_Time_Rec_Base() - smWaitTick)>30)	/*20 ms*/
		{
			SM_State =  SM_CONNECTING;
			currentMeterID = constMeterID[correntScanID];
			correntScanID++;
			/*move to next meter addreass*/
			//if(currentMeterID == 201)
			if(correntScanID == sizeof(constMeterID))
			{
				/*not fount any meter*/
				correntScanID = 0; /*reset to default*/
				smWaitTick = getVar_Time_Rec_Base();
				SM_State =  SM_WAIT;		
				meterStat = METER_ERR;
				aRxBuffer[0] = 0;
				aRxBuffer[1] = 0;
			}
		}
		break;
	case SM_CONNECTED:
		if((getVar_Time_Rec_Base() - smWaitTick)>150)	/*1500ms*/
		{
			SM_State =  SM_READ_METERID; 
		}
		break;
		
	case SM_READ_METERID:
		lastReadingCommand = 3;
		Make_PowermeterPacket(lastReadingCommand);
		ubRxIndex = 100 ;   
		ubTxIndex = 0 ; 
		
		/* Enable the Tx buffer empty interrupt */
		RS458_DIR_TX(); 
		USART_ITConfig(RS485_UART, USART_IT_TXE, ENABLE); // Read Data
		
		smWaitTick = getVar_Time_Rec_Base();
		
		SM_State =  SM_GET_DATA  ;   
		break;
	case SM_READ_METER_VOLT:
		lastReadingCommand = 4;
		Make_PowermeterPacket(lastReadingCommand);
		ubRxIndex = 100 ;   
		ubTxIndex = 0 ; 
		
		/* Enable the Tx buffer empty interrupt */
		RS458_DIR_TX(); 
		USART_ITConfig(RS485_UART, USART_IT_TXE, ENABLE); // Read Data
		
		
		smWaitTick = getVar_Time_Rec_Base();
		
		SM_State =  SM_GET_DATA  ;   
		break;
	
	case SM_READ_METER_CURRENT:
		lastReadingCommand = 5;
		Make_PowermeterPacket(lastReadingCommand);
		ubRxIndex = 100 ;   
		ubTxIndex = 0 ; 
		
		/* Enable the Tx buffer empty interrupt */
		RS458_DIR_TX(); 
		USART_ITConfig(RS485_UART, USART_IT_TXE, ENABLE); // Read Data
		
		
		smWaitTick = getVar_Time_Rec_Base();
		
		SM_State =  SM_GET_DATA  ;   
		break;
		
	case SM_READ_METER_WATT:
//		if((getVar_Time_Rec_Base() - smWaitTick)<150)	/*1500ms*/
//		{
//			break;
//		}
		
		Make_PowermeterPacket(1);
		ubRxIndex = 100 ;   
		ubTxIndex = 0 ; 
		
		/* Enable the Tx buffer empty interrupt */
		RS458_DIR_TX(); 
		USART_ITConfig(RS485_UART, USART_IT_TXE, ENABLE); // Read Data
		
		lastReadingCommand = 1;
		smWaitTick = getVar_Time_Rec_Base();
		
		SM_State =  SM_GET_DATA  ;   
		break;
	case SM_GET_DATA:
		if( ubRxIndex == 51  ) 
		{
			if(lastReadingCommand == 1)		/*last is reading total watt*/
			{
				DecodeEnergyValue();		
				SM_State =  SM_DISCONNECT ;  	
			}
			else if(lastReadingCommand == 3)	/*last is reading meter id*/
			{
				deCodeMeterSerialNumber();
				meterStat = METER_OK;
				SM_State =  SM_READ_METER_VOLT ;  
				//smWaitTick = getVar_Time_Rec_Base();	/*hard code for bug*/
			}
			else if(lastReadingCommand == 4)	/*last is reading voltage*/
			{
				deCodeMeterVoltage();
				meterStat = METER_OK;
				SM_State =  SM_READ_METER_CURRENT ;  
			}
			else if(lastReadingCommand == 5)	/*last is reading current*/
			{
				deCodeMeterCurrent();
				meterStat = METER_OK;
				SM_State =  SM_READ_METER_WATT ; 
			}
			else
			{
				SM_State =  SM_DISCONNECT ; 
			}
		}    
		/*checking timeout*/
		if((getVar_Time_Rec_Base() - smWaitTick)>150)	/*1500ms*/
		{
			SM_State =  SM_DISCONNECT ;  
			meterStat = METER_ERR;
		}	
		
		break; 			
	case SM_DISCONNECT:
		Make_PowermeterPacket(2);
		ubRxIndex = 100 ;   
		
		/* Enable the Tx buffer empty interrupt */
		RS458_DIR_TX();
		USART_ITConfig(RS485_UART, USART_IT_TXE, ENABLE); // DISCONNECT
		ubTxIndex = 0; 
		
		SM_State =  SM_WAIT;
		smWaitTick = getVar_Time_Rec_Base();
		break;
	case SM_WAIT:
		if((getVar_Time_Rec_Base() - smWaitTick)>500)
		{
			SM_State =  SM_CONNECTING; 
			aRxBuffer[0] = 0;
			aRxBuffer[1] = 0;
		}
		break; 
	}
}
/**
* @brief  Configures the USART Peripheral.
* @param  None
* @retval None
*/
static void RS485_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RS485_TX_GPIO_CLK | RS485_RX_GPIO_CLK, ENABLE);
	
	/* Enable USART clock */
	RS485_UART_CLK_INIT(RS485_UART_CLK, ENABLE);
	
	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(RS485_TX_GPIO_PORT, RS485_TX_SOURCE, RS485_TX_AF);
	GPIO_PinAFConfig(RS485_RX_GPIO_PORT, RS485_RX_SOURCE, RS485_RX_AF);
	
	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = RS485_TX_PIN;
	GPIO_Init(RS485_TX_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = RS485_RX_PIN;
	GPIO_Init(RS485_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable the USART OverSampling by 8 */
	USART_OverSampling8Cmd(RS485_UART, ENABLE);  
	
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
	USART_Init(RS485_UART, &USART_InitStructure);
	
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
	USART_Cmd(RS485_UART, ENABLE);
}


static bool CheckACK(void)
{
	int i = 0 , j = 0 ;
	
	
	ACKBuffer[0] = 0x3a;
	ACKBuffer[1] = currentMeterID;
	ACKBuffer[2] = 0x06;
	for(i=3;i<=47;i++)
	{   
		ACKBuffer[i]=0x23 ; 
		
    }
	
	*( ( unsigned short * )&ACKBuffer[48] ) = powermeter_calCrc( &ACKBuffer[1], 47 );	
	
	ACKBuffer[50] = 0x03;	
	
	
	while(j<51) {
		
		if(aRxBuffer[j]== ACKBuffer[j])
		{
			j++ ;
		}
		else
		{
			i=100;
			return  false ; 
		}
	}
	return  true ; 
	
}

static void DecodeEnergyValue(void)
{
	int i = 0  ;
	EnergyValue = 0 ; 
	
	
	DataBuffer[0] = aRxBuffer[6]&~(0xf0) ;
	DataBuffer[1] = aRxBuffer[7]&~(0xf0) ;
	DataBuffer[2] = aRxBuffer[8]&~(0xf0) ;
	DataBuffer[3] = aRxBuffer[9]&~(0xf0) ;
	DataBuffer[4] = aRxBuffer[10]&~(0xf0) ;
	DataBuffer[5] = aRxBuffer[11]&~(0xf0) ;
	DataBuffer[6] = aRxBuffer[12]&~(0xf0) ;
	DataBuffer[7] = aRxBuffer[13]&~(0xf0) ;
	DataBuffer[8] = aRxBuffer[14]&~(0xf0) ;
	
	
	DataBuffer[0] = DataBuffer[0] * 100000000.0 ;
	DataBuffer[1] = DataBuffer[1] * 10000000.0 ;
	DataBuffer[2] = DataBuffer[2] * 1000000.0 ;
	DataBuffer[3] = DataBuffer[3] * 100000.0 ;
	DataBuffer[4] = DataBuffer[4] * 10000.0 ;
	DataBuffer[5] = DataBuffer[5] * 1000.0 ;
	DataBuffer[6] = DataBuffer[6] * 100.0 ;
	DataBuffer[7] = DataBuffer[7] * 10.0 ;
	DataBuffer[8] = DataBuffer[8] * 1.0 ;
	
	for(i=0 ; i<9 ; i++)
	{
		EnergyValue += DataBuffer[i];
	}
	// EnergyValue = EnergyValue/1000 ;
}

unsigned char powermeter_calBcc( unsigned char *msg, int msg_len )
{
	unsigned char bcc = *msg;
	
	// Shift to next character
	msg++;
	msg_len--;
	
	while( msg_len > 0 )
	{
		bcc ^= *msg;
		
		// Shift to next character
		msg++;
		msg_len--;
	}
	
	return bcc;
}

unsigned short powermeter_calCrc( unsigned char *msg, int msg_len )
{
	unsigned char byte;
	int i, byte_flag, crc16_flag;
	
	unsigned short crc16;
	
	unsigned short polynomial = 0x1021;
	unsigned short seed = 0xffff;
	
	// CRC Standard: CRC16 CCITT
	crc16 = seed;
	
	while( msg_len > 0 )
	{
		byte = *msg;
		
		for( i = 0; i < 8; i++)
		{
			byte_flag = ( ( byte >> ( 7 - i ) ) & 0x00000001 );
			crc16_flag = ( ( crc16 >> 15 ) & 0x00000001 );
			
			crc16 <<= 1;
			
			if( byte_flag ^ crc16_flag )
			{
				crc16 ^= polynomial;
			}
		}
		
		// Shift to next character
		msg++;
		msg_len--;
	}
	
	return crc16;
}

//unsigned char powermeter_meterToRs485( unsigned char *meterId, int id_len )
//{
//	unsigned char currentMeterID[4];
//	
//	unsigned char digit_1 = *( meterId+id_len-1 );
//	unsigned char digit_2 = *( meterId+id_len-2 );
//	unsigned char digit_3 = *( meterId+id_len-3 );
//	
//	
//	if( digit_3 == '0' && digit_2 == '0' && digit_1 == '0' )
//	{
//		*( currentMeterID+( sizeof( currentMeterID )-1 )-1 ) = '0';
//		*( currentMeterID+( sizeof( currentMeterID )-1 )-2 ) = '0';
//		*( currentMeterID+( sizeof( currentMeterID )-1 )-3 ) = '2';
//	}
//	else
//	{
//		*( currentMeterID+( sizeof( currentMeterID )-1 )-1 ) = digit_1;
//		*( currentMeterID+( sizeof( currentMeterID )-1 )-2 ) = digit_2;
//		
//		if( ( digit_3 % 2 ) == 0 )
//		{
//			*( currentMeterID+( sizeof( currentMeterID )-1 )-3 ) = '0';					
//		}
//		else
//		{
//			*( currentMeterID+( sizeof( currentMeterID )-1 )-3 ) = '1';
//		}
//	}
//	
//	
//	return atoi((char const*) currentMeterID );
//}



void Make_PowermeterPacket( uint8_t type )
{
	// TX Data Template
	/*
	unsigned char rs485_msg = {
	0x3a, 0xff, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23,
	0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0xff, 0xff,
	0x03, };
	
	unsigned char connect_msg = {
	0x81, 0x50, 0xb1, 0x82, 0x28, 0xd2, 0x53, 0xb4, 
	0xb8, 0x35, 0xd4, 0xd7, 0xcf, 0xd7, 0xc9, 0xd2, 0xc5, 0x53, 
	0x50, 0xd2, 0xcf, 0xca, 0xc5, 0xc3, 0xd4, 0xa9, 0x03, 0xff, 
};
	
	unsigned char disconnect_msg = {
	0x81, 0x42, 0x30, 0x03, 0xff, 
};
	
	unsigned char req_EnergyValue = {
	0x81, 0xd2, 0xb2, 0x82, 0x44, 0xb7, 0x28, 0xa9,
	0x03, 0xff,
};
	*/
	
	
	//    CrcResult;
  	int i;
	
	//int rs485Id = 0x24;
	//int rs485Id = 0x60;
	
	
	
	switch( type )
	{
	case 0: // Connect Message
		{
			
			pSensorSoftBuffer[0] = 0x3a;
			pSensorSoftBuffer[1] = currentMeterID;
			
			pSensorSoftBuffer[2] = 0x81;
			pSensorSoftBuffer[3] = 0x50;
			pSensorSoftBuffer[4] = 0xb1;
			pSensorSoftBuffer[5] = 0x82;
			pSensorSoftBuffer[6] = 0x28;
			pSensorSoftBuffer[7] = 0xd2;
			pSensorSoftBuffer[8] = 0x53;
			pSensorSoftBuffer[9] = 0xb4;
			pSensorSoftBuffer[10] = 0xb8;
			pSensorSoftBuffer[11] = 0x35;
			pSensorSoftBuffer[12] = 0xd4;
			pSensorSoftBuffer[13] = 0xd7;
			pSensorSoftBuffer[14] = 0xcf;
			pSensorSoftBuffer[15] = 0xd7;
			pSensorSoftBuffer[16] = 0xc9;
			pSensorSoftBuffer[17] = 0xd2;
			pSensorSoftBuffer[18] = 0xc5;
			pSensorSoftBuffer[19] = 0x53;
			pSensorSoftBuffer[20] = 0x50;
			pSensorSoftBuffer[21] = 0xd2;
			pSensorSoftBuffer[22] = 0xcf;
			pSensorSoftBuffer[23] = 0xca;
			pSensorSoftBuffer[24] = 0xc5;
			pSensorSoftBuffer[25] = 0xc3;
			pSensorSoftBuffer[26] = 0xd4;
			pSensorSoftBuffer[27] = 0xa9;
			pSensorSoftBuffer[28] = 0x03;
			pSensorSoftBuffer[29] = powermeter_calBcc( &pSensorSoftBuffer[3], 26 );
			
			for(i=30;i<=47;i++)
			{   
				pSensorSoftBuffer[i]=0x23 ; 
				
			}
			
 			*( ( unsigned short * )&pSensorSoftBuffer[48] ) = powermeter_calCrc( &pSensorSoftBuffer[1], 47 );
			
 			pSensorSoftBuffer[50] = 0x03;
		} break;
		
	case 1: // Request Power Energy Value
		{
			
			pSensorSoftBuffer[0] = 0x3a;
			pSensorSoftBuffer[1] = currentMeterID;
			
			pSensorSoftBuffer[2] = 0x81;
			pSensorSoftBuffer[3] = 0xd2;
			pSensorSoftBuffer[4] = 0xb2;
			pSensorSoftBuffer[5] = 0x82;
			pSensorSoftBuffer[6] = 0x44;
			pSensorSoftBuffer[7] = 0xb7;
			pSensorSoftBuffer[8] = 0x28;
			pSensorSoftBuffer[9] = 0xa9;
			pSensorSoftBuffer[10] = 0x03;
			pSensorSoftBuffer[11] = powermeter_calBcc( &pSensorSoftBuffer[3], 8 );
			
			for(i=12;i<=47;i++)
			{   
				pSensorSoftBuffer[i]=0x23 ; 
				
			}
			
 			*( ( unsigned short * )&pSensorSoftBuffer[48] ) = powermeter_calCrc( &pSensorSoftBuffer[1], 47 );
 			
			pSensorSoftBuffer[50] = 0x03;
		} break;
		
	case 2: // Disconnect Message
		{
			
			
			pSensorSoftBuffer[0] = 0x3a;
			pSensorSoftBuffer[1] = currentMeterID;
			
			pSensorSoftBuffer[2] = 0x81;
			pSensorSoftBuffer[3] = 0x42;
			pSensorSoftBuffer[4] = 0x30;
			pSensorSoftBuffer[5] = 0x03;
			pSensorSoftBuffer[6] = powermeter_calBcc( &pSensorSoftBuffer[3], 3 );
			
			for(i=7;i<=47;i++)
			{   
				pSensorSoftBuffer[i]=0x23 ; 
				
			}
			
			*( ( unsigned short * )&pSensorSoftBuffer[48] ) = powermeter_calCrc( &pSensorSoftBuffer[1], 47 );	
			
			pSensorSoftBuffer[50] = 0x03;		
		} break;
		
		
	case 3: // read meter id
		{
			
			
			pSensorSoftBuffer[0] = 0x3a;
			pSensorSoftBuffer[1] = currentMeterID;
			
			pSensorSoftBuffer[2] = 0x81;
			pSensorSoftBuffer[3] = 0xD2;
			pSensorSoftBuffer[4] = 0xB2;
			pSensorSoftBuffer[5] = 0x82;
			pSensorSoftBuffer[6] = 0x30;
			pSensorSoftBuffer[7] = 0x30;
			pSensorSoftBuffer[8] = 0x28;
			pSensorSoftBuffer[9] = 0xA9;
			pSensorSoftBuffer[10] = 0x03;
			pSensorSoftBuffer[11] = 0x60;
			pSensorSoftBuffer[12] = powermeter_calBcc( &pSensorSoftBuffer[3], 9 );
			
			for(i=13;i<=47;i++)
			{   
				pSensorSoftBuffer[i]=0x23 ; 
				
			}
			
			*( ( unsigned short * )&pSensorSoftBuffer[48] ) = powermeter_calCrc( &pSensorSoftBuffer[1], 47 );	
			
			pSensorSoftBuffer[50] = 0x03;		
		} break;
	case 4: // Request voltage value
		{
			
			pSensorSoftBuffer[0] = 0x3a;
			pSensorSoftBuffer[1] = currentMeterID;
			
			pSensorSoftBuffer[2] = 0x81;
			pSensorSoftBuffer[3] = 0xd2;
			pSensorSoftBuffer[4] = 0xb2;
			pSensorSoftBuffer[5] = 0x82;
			pSensorSoftBuffer[6] = 0x44;
			pSensorSoftBuffer[7] = 0x30;
			pSensorSoftBuffer[8] = 0x28;
			pSensorSoftBuffer[9] = 0xa9;
			pSensorSoftBuffer[10] = 0x03;
			pSensorSoftBuffer[11] = powermeter_calBcc( &pSensorSoftBuffer[3], 8 );
			
			for(i=12;i<=47;i++)
			{   
				pSensorSoftBuffer[i]=0x23 ; 
				
			}
			
 			*( ( unsigned short * )&pSensorSoftBuffer[48] ) = powermeter_calCrc( &pSensorSoftBuffer[1], 47 );
 			
			pSensorSoftBuffer[50] = 0x03;
		} break;
	case 5: // Request current
		{
			
			pSensorSoftBuffer[0] = 0x3a;
			pSensorSoftBuffer[1] = currentMeterID;
			
			pSensorSoftBuffer[2] = 0x81;
			pSensorSoftBuffer[3] = 0xd2;
			pSensorSoftBuffer[4] = 0xb2;
			pSensorSoftBuffer[5] = 0x82;
			pSensorSoftBuffer[6] = 0x44;
			pSensorSoftBuffer[7] = 0xB2;
			pSensorSoftBuffer[8] = 0x28;
			pSensorSoftBuffer[9] = 0xa9;
			pSensorSoftBuffer[10] = 0x03;
			pSensorSoftBuffer[11] = powermeter_calBcc( &pSensorSoftBuffer[3], 8 );
			
			for(i=12;i<=47;i++)
			{   
				pSensorSoftBuffer[i]=0x23 ; 
				
			}
			
 			*( ( unsigned short * )&pSensorSoftBuffer[48] ) = powermeter_calCrc( &pSensorSoftBuffer[1], 47 );
 			
			pSensorSoftBuffer[50] = 0x03;
		} break;
	default:
		break;
	}	
}



/**
* @brief  This function handles USRAT interrupt request.
* @param  None
* @retval None
*/
void RS485_IRQHandler(void)
{
	
	/* USART in Receiver mode */
	if (USART_GetITStatus(RS485_UART, USART_IT_RXNE) == SET)
	{
		if (ubRxIndex < BUFFERSIZE)
		{
			
			/* Receive Transaction data */
			aRxBuffer[ubRxIndex++] = USART_ReceiveData(RS485_UART);
			
		}
		else
		{
			/* Disable the Rx buffer not empty interrupt */
			USART_ITConfig(RS485_UART, USART_IT_RXNE, DISABLE);
			
		}
	}
	
	
	
	
	
	/* USART in Tramitter mode */
	if (USART_GetITStatus(RS485_UART, USART_IT_TXE) == SET)
	{
		if (ubTxIndex < BUFFERSIZE)
		{
			/* Send Transaction data */
			USART_SendData(RS485_UART, pSensorSoftBuffer[ubTxIndex]);
			ubTxIndex++;
		}
		else
		{           
			RS458_DIR_RX();
			/* Disable the Tx buffer empty interrupt */
			USART_ITConfig(RS485_UART, USART_IT_TXE, DISABLE);
			//Status_SendData ++ ;  
			ubRxIndex = 0 ;  
			/* Enable the 	RX buffer empty interrupt */
			USART_ITConfig(RS485_UART, USART_IT_RXNE, ENABLE);	
		}
	}
}
