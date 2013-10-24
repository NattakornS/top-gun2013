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
-- Filename     main_abEvalTest.c
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
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sx1App.h"
#include "oledApp.h"
#include "dgZigbeeApi.h"
#include "dgSIC4310App.h"
#include "dgSaijoApp.h"
#include "stm32f4xx_it.h"
#include BOARD_DES_H



/** @addtogroup STM32F4xx_StdPeriph_Examples
* @{
*/

/** @addtogroup USART_DataExchangeInterrupt
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
uint8_t BlueBtnPressed = 0x00;
/* Private function prototypes -----------------------------------------------*/
//static void SysTickConfig(void);


/* Private functions ---------------------------------------------------------*/
static void abEval_AllBoardInit(void);
static void abEval_UIInit(void);

static void appDisplayInit(void);

static void appDisplayProcess(void);
static void dgZigbeeTick(void);
static void dgZigbeeTick_Air(void);
static void dgZigbeeTick_Meter(void);
int Convert(char * str);
// Calculate Power -------------------------------------------------------------
uint16_t Init_Power;
void Calculate_Power(void);
void Money (void);
uint8_t i;
int Energy_Init = 0, Power_Use = 0;
float Cost_Power = 0;
float Remain = 0;
extern int EnergyValue;
char Cost_Power_Ch[10];
char Remain_Ch[10];
// Create Variable for Money ---------------------------------------------------
unsigned char Money_Input[10];
uint16_t Money_InSystem;
char energyBuffZb[32];
char AirBuffZb[32];
char MeterBuffZb[32];
int  counter_report=0;

/**
* @brief  Main program
* @param  None
* @retval None
*/
int main(void)
{
    int i = 0;
    /* SysTick end of count event each 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
	
	/*!< At this stage the microcontroller clock setting is already configured, 
	this is done through SystemInit() function which is called from startup
	files (startup_stm32f40xx.s/startup_stm32f427x.s) before to branch to 
	application main. 
	To reconfigure the default setting of SystemInit() function, refer to
	system_stm32f4xx.c file
	*/

	abEval_AllBoardInit();
	
	while (1)
	{
		/* Relay Control & Dryin Monitor */
		abEval_IOProcess();
		
		/* E-Meter Monitor */
		sx1App_Process();
		
		/* OLED Display */
		appDisplayProcess();
		
		/* Zigbee Incoming Handler */
		dgZigbeeApiIncomingHandler();
		
		/* Zigbee Reporting */
		//dgZigbeeTick();
		dgZigbeeTick_Air();    
                dgZigbeeTick_Meter();
              //  counter_report++; 
              //  if(counter_report>10000) {counter_report=0;}
		
		/* Saijo Status Monitoring*/
		dgSaijoIncomingHandler();
		
		/* NFC Monitoring*/
		dgSIC4310IncomingHandler();
		

               
		if(BlueBtnPressed == 0x01)
		{
			BlueBtnPressed = 0x00;
			abEval_RelaySet(RELAY1, (abEval_getRelay(RELAY1)==RELAY_ST_ON)?RELAY_ST_OFF:RELAY_ST_ON);
		}
	}
}

static void abEval_AllBoardInit(void)
{
	abEval_UIInit();
	abEval_IOInit();

	Oled_SSD1305_Init();
	
	sx1App_Init();
	appDisplayInit();
	dgSaijoInit();
	dgZigbeeApiInit();
	dgSic4310Init();	/* NFC */
}

static void abEval_UIInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the GPIO_LED Clock */
	RCC_AHB1PeriphClockCmd(LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK | LED4_GPIO_CLK , ENABLE);
	
	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED3_PIN;
	GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED4_PIN;
	GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStructure);
	
	/* Enable the BLUE_BTN */
	{
		/* Enable the BUTTON Clock */
		RCC_AHB1PeriphClockCmd(BLUE_BTN_CLK, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		
		/* Configure Button pin as input */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = BLUE_BTN_PIN;
		GPIO_Init(BLUE_BTN_PORT, &GPIO_InitStructure);
		
		/* Connect Button EXTI Line to Button GPIO Pin */
		SYSCFG_EXTILineConfig(BLUE_BTN_EXTI_PORT_SOURCE, BLUE_BTN_EXTI_PIN_SOURCE);
		
		/* Configure Button EXTI line */
		EXTI_InitStructure.EXTI_Line = BLUE_BTN_EXTI_LINE;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
		
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/* Enable and set Button EXTI Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = BLUE_BTN_EXTI_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		
		NVIC_Init(&NVIC_InitStructure); 
	}
}

static void dgZigbeeTick(void)
{	
	static uint16_t zbTick = 0;
	unsigned int tmpE = 0;
	if((getVar_Time_Rec_Base()-zbTick)>500)
	{
//		energyBuffZb[0] = 'e';
//		*(unsigned int*)&energyBuffZb[1] = getVar_EnergyValue();
		
                  
            if(strcmp("Auto   ",getAirFanSpeed())==0){tmpE=0;}
            else if (strcmp("Low    ",getAirFanSpeed())==0){tmpE=1;}
            else if (strcmp("Medium ",getAirFanSpeed())==0){tmpE=2;}
            else if (strcmp("Hi     ",getAirFanSpeed())==0){tmpE=3;}
            else if (strcmp("HiHi   ",getAirFanSpeed())==0){tmpE=4;}
            else if (strcmp("Turbo  ",getAirFanSpeed())==0){tmpE=5;}
            else if (strcmp("Off    ",getAirFanSpeed())==0){tmpE=6;}

            if(isAirPowerON()==0){tmpE=6;}
            sprintf(energyBuffZb, "a%d,%d,0,0\0\n", isAirPowerON(),tmpE);
	    dgZigbeeApiSendString(0, energyBuffZb);
                
          
//                tmpE = getVar_EnergyValue();
//		sprintf(energyBuffZb, "e%d.%03d\0", tmpE/1000, tmpE%1000);
//		
//		dgZigbeeApiSendString(0, energyBuffZb);
//		
                /*Reset zbTick*/
		zbTick = getVar_Time_Rec_Base();
	}
}


static void dgZigbeeTick_Air(void)
{	
	static uint16_t zbTick = 0;
	unsigned int tmpE = 0;
        int a,len;
      //  len=strlen(getAirFanSpeed());
	if((getVar_Time_Rec_Base()-zbTick)>100 && (counter_report%2==0))
	{       
            isAirPowerON();
            if(strcmp("Auto   ",getAirFanSpeed())==0){tmpE=0;}
            else if (strcmp("Low    ",getAirFanSpeed())==0){tmpE=1;}
            else if (strcmp("Medium ",getAirFanSpeed())==0){tmpE=2;}
            else if (strcmp("Hi     ",getAirFanSpeed())==0){tmpE=3;}
            else if (strcmp("HiHi   ",getAirFanSpeed())==0){tmpE=4;}
            else if (strcmp("Turbo  ",getAirFanSpeed())==0){tmpE=5;}
            else if (strcmp("Off    ",getAirFanSpeed())==0){tmpE=6;}

            if(isAirPowerON()==0){tmpE=6;}
            sprintf(AirBuffZb, "a%d,%d,0,0\0\n", isAirPowerON(),tmpE);
	    dgZigbeeApiSendString(0, AirBuffZb);
                
		/*Reset zbTick*/
		zbTick = getVar_Time_Rec_Base();
	}
}


static void dgZigbeeTick_Meter()
{	
	static uint16_t zbTick = 0;
	unsigned int tmpE = 0;
        int a,len;
 
	if(((getVar_Time_Rec_Base()-zbTick)>1000) && (counter_report%2==1))
	{       
         
            sprintf(MeterBuffZb,"b%d,%d,%d,%d,%s\0",getMeterId(),getVar_EnergyValue,getMeterVoltage(),getMeterCurrent(),Remain_Ch);
	    dgZigbeeApiSendString(0, MeterBuffZb);
                
		/*Reset zbTick*/
		zbTick = getVar_Time_Rec_Base();
	}
}



static void appDisplayInit(void)
{
	lcd_Str(0, 0, "MU Brain");
	//lcd_Str(0, 1, "----------------");
	
	//lcd_Str(10, 2," kWh");
	//lcd_Str(0, 2, "Meter ID NO.");
	//lcd_Str(2, 3, METER_ID);
	
	//lcd_Str(0, 5, "Stat: ");
//	lcd_Str(0, 6, "Air: ");
//	lcd_Str(0, 7, "O:?? I:?? -----");
}
char* statusStr[4] = {
	"___",
	"__^",
	"_^_",
	"^__",
};


static void appDisplayProcess(void)
{
	static uint16_t dpTick = 0;
	static int Count = 0 ;
	
	uint8_t tmpPrint[24];
	
	if((getVar_Time_Rec_Base()-dpTick)>100)
	{
		/* 5th Line Updating */
//		display_valiable(12, 5, Count++, 10, 2, 0, 0);
//		lcd_Str(12, 5, statusStr[Count]);
//		Count = (Count+1)%4;
		
//		switch(getVar_meterStatus())
//		{
//		case METER_OK:
//		case METER_ERR:
//		default:
//			lcd_Str(5, 5, "OK");
//		}
		/*line 2 meter status*/
		if(getVar_meterStatus() == METER_OK)
		{
			sprintf(tmpPrint,"Meter ID %07d",getMeterId());	
			
			/*power*/
			lcd_Str(0, 2,"P = ");
			display_valiable(4, 2, (getVar_EnergyValue()), 10, 9, 1, 3);
			if(i < 20)
                        {
                          Energy_Init = EnergyValue;
                          i++;
                        }
//                        lcd_Str(0, 3,"P_ini= ");
//                        display_valiable(5, 3, Energy_Init, 10, 9, 1, 3);
                        Calculate_Power();
                        Money();
                        lcd_Str(0, 6,"Remain:");
                        lcd_Str(8, 6,Remain_Ch);
                        lcd_Str(0, 7,"Cost: ");
                        lcd_Str(6, 7,Cost_Power_Ch);
                        
//			lcd_Str(0, 3,"V:");
//			display_valiable(2, 3, getMeterVoltage(), 10, 5, 1, 2);
//			lcd_Str(9, 3,"A:");
//			display_valiable(11, 3, getMeterCurrent(), 10, 3, 0, 2);
		}
		else
		{
			sprintf(tmpPrint,"Meter N/C       ");	
			
			/*power*/
			lcd_Str(0, 2,"                ");
			
			/*v&i*/
			lcd_Str(0, 3,"                ");
		}
		lcd_Str(0, 1, tmpPrint);
		
		
		/*air status*/
		if(isAirPowerON())
		{
			lcd_Str(0, 4,"Air Status : ON ");
			lcd_Str(0, 5,"Fan :");
			lcd_Str(5, 5,getAirFanSpeed());
		}
		else
		{
			lcd_Str(0, 4,"Air Status : OFF");
			lcd_Str(0, 5,"                ");
		}
		
		/*NFC Status*/
		if(IsNFCUpdated())
		{
			/* Update data from NFC */
			lcd_Str( 0, 6, "                ");
			strcpy(tmpPrint, "");
			strncpy(tmpPrint, sic4310RxBuff, sizeof(tmpPrint)-1);
                        lcd_Str( 0, 3, "Paid: ");
			lcd_Str( 7, 3, sic4310RxBuff);  //sic4310RxBuff
                        uint8_t x;
                        for(x=0;x<10;x++)
                        {
                          Money_Input[x] = sic4310RxBuff[x];
                          //Energy_Init = EnergyValue;
                          Money();
                        }
                        for(x=0;x<64;x++)
                        {  
                          
                          sic4310RxBuff[x] = 0;
			}
			/*Clear flag and buffer after using*/
			clr_FlagNFCUpdated();
		}
//		/* 6th Line Updating :Air status*/
//		lcd_Str(5, 6, getVar_dgSaijo_Status()?"ON ":"OFF");
//		/* 7th Line IO Status*/
//		{
//			char lcdStr[16+1];
//			sprintf(lcdStr, "O:%d%d I:%c%c -----",
//					abEval_getRelay(RELAY1),
//					abEval_getRelay(RELAY2),
//					abEval_getDryin(DRYIN1)?'Z':'0',
//					abEval_getDryin(DRYIN2)?'Z':'0');
//			lcd_Str(0, 7, lcdStr);
//		}
//		dpTick = getVar_Time_Rec_Base();
//		if(dpTick > 0xFF90)	
//		{
//			dpTick = 0;
//		}
	}
}

	





#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	
	/* Infinite loop */
	while (1)
	{}
}
#endif

void Calculate_Power(void)
{
  Power_Use = EnergyValue - Energy_Init;
  if(Power_Use < 15)
  {
    Cost_Power = (float)Power_Use * 1.8632;
  }
  else if(Power_Use > 16 & Power_Use < 35)
  {
    Cost_Power = (15 * 1.8632) + ((Power_Use - 15)*2.5026);
  }
 else if(Power_Use > 26 & Power_Use < 35)
  {
    Cost_Power = (15 * 1.8632) + ((10)*2.5026) + ((Power_Use - 25)*2.7549);
  }
  else if(Power_Use > 36 & Power_Use < 100)
  {
    Cost_Power = (15 * 1.8632) + ((10)*2.5026) + (10*2.7549) + ((Power_Use - 35)*3.1381);
  }
  else if(Power_Use > 101 & Power_Use < 150)
  {
    Cost_Power = (15 * 1.8632) + ((10)*2.5026) + (10*2.7549) + (50*3.1381) + ((Power_Use - 100)*3.2315);
  } 
   else if(Power_Use > 151 & Power_Use < 400)
  {
    Cost_Power = (15 * 1.8632) + ((10)*2.5026) + (10*2.7549) + (50*3.1381) + (65*3.2315) + ((Power_Use - 150)*3.7362);
  } 
   else if(Power_Use > 401)
  {
    Cost_Power = (15 * 1.8632) + ((10)*2.5026) + (10*2.7549) + (50*3.1381) + (65*3.2315) + (250*3.7362) + ((Power_Use - 400)*3.9361);
  }
  //Calculate FT Cost
  Cost_Power = Cost_Power + (Power_Use*0.54);
  //Calculate Vat
  Cost_Power = Cost_Power*1.07;
  //Convert Float to String  //12345  12.345
  Cost_Power_Ch[0] = '0'+Cost_Power/1000;
  Cost_Power_Ch[1] = '0'+((uint32_t)Cost_Power%1000)/100;
  Cost_Power_Ch[2] = '0'+((uint32_t)Cost_Power%100)/10;
  Cost_Power_Ch[3] = '0'+((uint32_t)Cost_Power%10)/1;
  Cost_Power_Ch[4] = '.';
  Cost_Power_Ch[5] = '0'+((uint32_t)((Cost_Power)*10.0))%10;
  Cost_Power_Ch[6] = '0'+((uint32_t)((Cost_Power)*100.0))%10;
  Cost_Power_Ch[7] = '0'+((uint32_t)((Cost_Power)*1000.0))%10;
  Cost_Power_Ch[8] = 0x00;
  
}
// Function Convert String to int
void Money(void)
{
  Money_InSystem = atoi(Money_Input);
  Remain = (float)Money_InSystem - Cost_Power;
  //Convert Float to String  //12345  12.345
  Remain_Ch[0] = '0'+Remain/1000;
  Remain_Ch[1] = '0'+((uint32_t)Remain%1000)/100;
  Remain_Ch[2] = '0'+((uint32_t)Remain%100)/10;
  Remain_Ch[3] = '0'+((uint32_t)Remain%10)/1;
  Remain_Ch[4] = '.';
  Remain_Ch[5] = '0'+((uint32_t)((Remain)*10.0))%10;
  Remain_Ch[6] = '0'+((uint32_t)((Remain)*100.0))%10;
  Remain_Ch[7] = '0'+((uint32_t)((Remain)*1000.0))%10;
  Remain_Ch[8] = 0x00;
  if(Remain <= 0)
  {
    dgSaijo_Power_Off();
  }
}
int Convert(char * str)
{
    int result =0;
    int len=strlen(str);
    for(int i=0,j=len-1;i<len;i++,j--)
    {
      result += ((int)str[i] - 48)*pow(10,j);
    }

   return result;
}
/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
