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

#ifndef _SX1_APP_H
#define _SX1_APP_H

//#define METER_ID						"2704236"
//#define METERID					0x60


#define BUFFERSIZE						64

//#define START_SCAN			90
typedef enum{
	METER_OK,
	METER_ERR,
}METER_STATUS;

void sx1App_Init(void);
void sx1App_Process(void);

METER_STATUS getVar_meterStatus(void);
int getVar_EnergyValue(void);
uint32_t	getMeterId();
uint32_t getMeterVoltage();
uint32_t getMeterCurrent();
void RS485_IRQHandler(void);
#endif	/* _SX1_APP_H */