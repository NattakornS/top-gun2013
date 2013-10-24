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
-- Filename     tesaDemoBoard.h
-- Company      Design Gateway Co.,Ltd
-- Syntax       H
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


#ifndef __TESA_DEMO_BOARD_H
#define __TESA_DEMO_BOARD_H

#define nDryin				2
#define nRelay				2
#define nDip				2

#define RELAY1_PIN			GPIO_Pin_8
#define RELAY1_PORT			GPIOB
#define RELAY1_CLK          RCC_AHB1Periph_GPIOB

#define RELAY2_PIN          GPIO_Pin_9
#define RELAY2_PORT         GPIOB
#define RELAY2_CLK          RCC_AHB1Periph_GPIOB

#define DRYIN1_PIN              	GPIO_Pin_0
#define DRYIN1_PORT             	GPIOB
#define DRYIN1_CLK              	RCC_AHB1Periph_GPIOB
#define DRYIN1_EXTI_LINE			EXTI_Line0
#define DRYIN1_EXTI_PORT_SOURCE		EXTI_PortSourceGPIOB
#define DRYIN1_EXTI_PIN_SOURCE		EXTI_PinSource0
#define DRYIN1_EXTI_IRQn			EXTI0_IRQn
#define DRYIN1_IRQHandler			EXTI0_IRQHandler

#define DRYIN2_PIN              	GPIO_Pin_10
#define DRYIN2_PORT             	GPIOB
#define DRYIN2_CLK              	RCC_AHB1Periph_GPIOB
#define DRYIN2_EXTI_LINE			EXTI_Line10
#define DRYIN2_EXTI_PORT_SOURCE		EXTI_PortSourceGPIOB
#define DRYIN2_EXTI_PIN_SOURCE		EXTI_PinSource10
#define DRYIN2_EXTI_IRQn			EXTI15_10_IRQn
#define DRYIN2_IRQHandler			EXTI15_10_IRQHandler


#define DIP_SW1_PIN              	GPIO_Pin_3
#define DIP_SW1_PORT             	GPIOE
#define DIP_SW1_CLK              	RCC_AHB1Periph_GPIOE
//#define DIP_SW1_EXTI_LINE			EXTI_Line3
//#define DIP_SW1_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOE
//#define DIP_SW1_EXTI_PIN_SOURCE		EXTI_PinSource3
//#define DIP_SW1_EXTI_IRQn			EXTI3_IRQn
//#define DIP_SW1_IRQHandler			EXTI0_IRQHandler

#define DIP_SW2_PIN              	GPIO_Pin_4
#define DIP_SW2_PORT             	GPIOE
#define DIP_SW2_CLK              	RCC_AHB1Periph_GPIOE
//#define DIP_SW2_EXTI_LINE			EXTI_Line10
//#define DIP_SW2_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOB
//#define DIP_SW2_EXTI_PIN_SOURCE		EXTI_PinSource10
//#define DIP_SW2_EXTI_IRQn			EXTI15_10_IRQn
//#define DIP_SW2_IRQHandler			EXTI15_10_IRQHandler

//BLUE Botton
#define BLUE_BTN_PIN              	GPIO_Pin_0
#define BLUE_BTN_PORT             	GPIOA
#define BLUE_BTN_CLK              	RCC_AHB1Periph_GPIOA
#define BLUE_BTN_EXTI_LINE			EXTI_Line0
#define BLUE_BTN_EXTI_PORT_SOURCE	EXTI_PortSourceGPIOA
#define BLUE_BTN_EXTI_PIN_SOURCE	EXTI_PinSource0
#define BLUE_BTN_EXTI_IRQn			EXTI0_IRQn 
//#define BLUE_BTN_IRQHandler		EXTI0_IRQHandler

//Define RS485
#define RS485_UART					USART3
#define RS485_UART_CLK				RCC_APB1Periph_USART3
#define RS485_UART_CLK_INIT			RCC_APB1PeriphClockCmd
#define RS485_IRQn					USART3_IRQn
#define RS485_IRQHandler			USART3_IRQHandler


#define RS485_TX_PIN                    GPIO_Pin_8                
#define RS485_TX_GPIO_PORT              GPIOD                       
#define RS485_TX_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define RS485_TX_SOURCE                 GPIO_PinSource8
#define RS485_TX_AF                     GPIO_AF_USART3

#define RS485_RX_PIN                    GPIO_Pin_9                
#define RS485_RX_GPIO_PORT              GPIOD                    
#define RS485_RX_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define RS485_RX_SOURCE                 GPIO_PinSource9
#define RS485_RX_AF                     GPIO_AF_USART3

#define RS485_DIR_PIN                   GPIO_Pin_8                
#define RS485_DIR_GPIO_PORT             GPIOA                    
#define RS485_DIR_GPIO_CLK              RCC_AHB1Periph_GPIOA


#define RS458_DIR_RX()	{ RS485_DIR_GPIO_PORT->BSRRH = RS485_DIR_PIN; }
#define RS458_DIR_TX()	{ RS485_DIR_GPIO_PORT->BSRRL = RS485_DIR_PIN; }


#define OLED_I2C
#define OLED_SCL_PORT
#define OLED_SCL_PIN
#define OLED_SDA_PORT
#define OLED_SDA_PIN
#define OLED_RST_PORT
#define OLED_RST_PIN

#define ZB_UART					USART6
#define ZB_UART_CLK				RCC_APB2Periph_USART6
#define ZB_UART_CLK_INIT		RCC_APB2PeriphClockCmd
#define ZB_IRQn					USART6_IRQn
#define ZB_IRQHandler			USART6_IRQHandler


#define ZB_TX_PIN               GPIO_Pin_6
#define ZB_TX_GPIO_PORT         GPIOC
#define ZB_TX_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ZB_TX_SOURCE            GPIO_PinSource6
#define ZB_TX_AF                GPIO_AF_USART6

#define ZB_RX_PIN               GPIO_Pin_7                
#define ZB_RX_GPIO_PORT         GPIOC                    
#define ZB_RX_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ZB_RX_SOURCE            GPIO_PinSource7
#define ZB_RX_AF                GPIO_AF_USART6



//-----------------------------------------------------------//
#define SAIJO_UART				USART2
#define SAIJO_UART_CLK			RCC_APB1Periph_USART2
#define SAIJO_UART_CLK_INIT		RCC_APB1PeriphClockCmd
#define SAIJO_IRQn				USART2_IRQn
#define SAIJO_IRQHandler		USART2_IRQHandler


#define SAIJO_TX_PIN               GPIO_Pin_2
#define SAIJO_TX_GPIO_PORT         GPIOA
#define SAIJO_TX_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define SAIJO_TX_SOURCE            GPIO_PinSource2
#define SAIJO_TX_AF                GPIO_AF_USART2

#define SAIJO_RX_PIN               GPIO_Pin_3                
#define SAIJO_RX_GPIO_PORT         GPIOA                    
#define SAIJO_RX_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define SAIJO_RX_SOURCE            GPIO_PinSource3
#define SAIJO_RX_AF                GPIO_AF_USART2
//-----------------------------------------------------------//
#define SIC4310_UART		  		UART4
#define SIC4310_UART_CLK	    	RCC_APB1Periph_UART4
#define SIC4310_UART_CLK_INIT       RCC_APB1PeriphClockCmd
#define SIC4310_IRQn		    	UART4_IRQn
#define SIC4310_IRQHandler	   		UART4_IRQHandler


#define SIC4310_TX_PIN               GPIO_Pin_0
#define SIC4310_TX_GPIO_PORT         GPIOA
#define SIC4310_TX_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define SIC4310_TX_SOURCE            GPIO_PinSource0
#define SIC4310_TX_AF                GPIO_AF_UART4

#define SIC4310_RX_PIN               GPIO_Pin_1              
#define SIC4310_RX_GPIO_PORT         GPIOA                    
#define SIC4310_RX_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define SIC4310_RX_SOURCE            GPIO_PinSource1
#define SIC4310_RX_AF                GPIO_AF_UART4

#endif	/* __TESA_DEMO_BOARD_H */
