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
-- Filename     oledApp.c
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
#include <stdbool.h>

#include "main.h"
#include "stm32f4xx_i2c.h"
#include BOARD_DES_H

#include "oledApp.h"
#include "dgSIC4310App.h"
// Letters font
#include "letters.h"	//97 letters x 6 Byte/1letter = 582 Byte 

/* Undefined below to make oled use 12V */
#define USE_OLED_CHG_PUMP

void Init_ReSetPin();
void init_I2C1();
void Oled_SSD1305_Init();
void I2C_start(I2C_TypeDef* , uint8_t , uint8_t );
void I2C_write(I2C_TypeDef* , uint8_t );
void write_I2C_command(uint8_t );
void write_I2C_command2(uint8_t ,uint8_t );
void I2C_stop(I2C_TypeDef* );
void all_clear();
void delay(unsigned int );
char *itoazz( int value, char buff[], int radix ) ;
char *mid(char *ds, char *ss, int a, int b);

void display_one_letter (unsigned char z, unsigned char column, unsigned char line);
void display_big (unsigned char z, unsigned char column, unsigned char line);
void lcd_Str(char colum, char line, char *s);
void lcd_Str_big(char colum, char line, char *s);
void display_valiable( unsigned int x_posi,unsigned int y_posi,long x, int radix, int col, int d, int dot);

static void delay(unsigned int );


void Oled_SSD1305_Init(){
	
	Init_ReSetPin();
	
	//OLED reset    
	
	/* Set PD6 */
	GPIOD->BSRRL = GPIO_Pin_6 ;
	delay(10);
	/* Reset PD6 */
	GPIOD->BSRRH = GPIO_Pin_6 ;
	delay(10);
	/* Set PD6 */
	GPIOD->BSRRL = GPIO_Pin_6 ;
	
	
	
	init_I2C1(); // initialize I2C peripheral
	
	
	//	uint8_t received_data[2];
	
	////SSD1305 registor setting
	
	write_I2C_command(0x00); 							        // Set Lower Column Address
	write_I2C_command(0x10); 								// Set Higher Column Address
	
	write_I2C_command(0x40); 								// Set Display Start Line
	#ifdef USE_OLED_CHG_PUMP
	write_I2C_command2(0x81,0xCF);							// Set Contrast Control (2Byte setting)
	#else
	write_I2C_command2(0x81,0x30);							// Set Contrast Control (2Byte setting)
	#endif
	// 0 ~ 127
	//Re-map
	//	write_I2C_command(0xA0); 							// Not remap Column
	write_I2C_command(0xA1); 								// Remap Column
	write_I2C_command(0xC8); 								// Not remap Line
	
	write_I2C_command(0xA4); 								// A4=ON
	
	write_I2C_command(0xA6);  								// Normal Displa(Not inverse)
	
	write_I2C_command2(0xA8,0x3f);							// Set Multiplex Ratio (2Byte setting)
	
	write_I2C_command2(0xAD,0x8A);							// Set DC-DC (2Byte setting)
	// 8B=ON, 8A=Off
	
	write_I2C_command(0xAF);  								// Display ON/OFF (AF=ON , AE=OFF)
	
	write_I2C_command2(0xD3,0x00);							// Set Display Offset (2Byte setting)
	// No offset
	
	write_I2C_command2(0xD5,0x61);							// Set Clock Divide (2Byte setting)
	// Set to 80Hz
	
	write_I2C_command2(0xD8,0x00);							// Set Area Color On or Off (2Byte setting)
	// Mono Mode
	
	write_I2C_command2(0xDA,0x12);							// Set Pins Hardware (2Byte setting)
	
	write_I2C_command2(0xDB,0x00);							// Set VCOMH (2Byte setting) 
	
	write_I2C_command2(0xD9,0x22);							// Set VP (2Byte setting)
	// P1=2 , P2=2
#ifdef USE_OLED_CHG_PUMP
	write_I2C_command2(0x8D,0x14);                                   // Charge Pump
#endif	
	write_I2C_command2(0x20,0x00);			                                /*Set address mode to horizontal mode*/
	
	all_clear();  //Clear display
	
}



void Init_ReSetPin(void){
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	/* Configure PG6 and PG8 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}


void init_I2C1(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	
	// enable APB1 peripheral clock for I2C1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	// enable clock for SCL and SDA pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* setup SCL and SDA pins
	* You can connect I2C1 to two different
	* pairs of pins:
	* 1. SCL on PB6 and SDA on PB7 
	* 2. SCL on PB8 and SDA on PB9
	*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;    // we are going to use PB6 and PB7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;		// set pins to alternate function
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;	// set GPIO speed
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;	        // set output to open drain --> the line has to be only pulled low, not driven high
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	        // enable pull up resistors
	GPIO_Init(GPIOB, &GPIO_InitStruct);		        // init GPIOB
	
	// Connect I2C1 pins to AF  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);	// SCL
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);       // SDA
	
	// configure I2C1 
	I2C_InitStruct.I2C_ClockSpeed = 400000 ; 		// 400kHz
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;		// I2C mode
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;		// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;		// disable acknowledge when reading (can be changed later on)
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses
	I2C_Init(I2C1, &I2C_InitStruct);				// init I2C1
	
	// enable I2C1
	I2C_Cmd(I2C1, ENABLE);
	
}



/* This function issues a start condition and 
* transmits the slave address + R/W bit
* 
* Parameters:
* 		I2Cx --> the I2C peripheral e.g. I2C1
* 		address --> the 7 bit slave address
* 		direction --> the tranmission direction can be:
* 						I2C_Direction_Tranmitter for Master transmitter mode
* 						I2C_Direction_Receiver for Master receiver
*/
void I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction){
	
	
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	
	// Send I2C1 START condition 
	I2C_GenerateSTART(I2Cx, ENABLE);
	
	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	// Send slave Address for write 
	I2C_Send7bitAddress(I2Cx, address, direction);
	
	/* wait for I2C1 EV6, check if 
	* either Slave has acknowledged Master transmitter or
	* Master receiver mode, depending on the transmission
	* direction
	*/	
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	
}


/* This function transmits one byte to the slave device
* Parameters:
*		I2Cx --> the I2C peripheral e.g. I2C1 
*		data --> the data byte to be transmitted
*/
void I2C_write(I2C_TypeDef* I2Cx, uint8_t data){
	I2C_SendData(I2Cx, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}



//Write command with parameter
void write_I2C_command2(uint8_t command,uint8_t parameter)	//Send command and parameter
{
	
	I2C_start(I2C1, OLED_I2C_ADDRESS ,I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	
	I2C_write(I2C1,0x80); //More than 2 Commands + D/C(L) +"0" x 6 pcs
	I2C_write(I2C1,command);
	I2C_write(I2C1,0x80);  //More than 2 Commands + D/C(L) +"0" x 6 pcs
	I2C_write(I2C1,parameter);    
	
	
	I2C_stop(I2C1); // stop the transmission
	
}//End of write_I2C_command2




//Write command
void write_I2C_command(uint8_t command)	//Send command only
{
	
	I2C_start(I2C1, OLED_I2C_ADDRESS ,I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	
	I2C_write(I2C1,0x80);                                         //More than 2 Commands + D/C(L) +"0" x 6 pcs
	I2C_write(I2C1,command);
	
	I2C_stop(I2C1);                                                // stop the transmission
	
}//End of write_I2C_command




//Write data
void write_I2C_data(uint8_t data)			
{
	I2C_start(I2C1, OLED_I2C_ADDRESS ,I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	
	
	I2C_write(I2C1,0x40);  //Only one data + D/C(L) +"0" x 6 pcs
	I2C_write(I2C1,data);
	
	I2C_stop(I2C1); // stop the transmission
	
}//End of write_I2C_data





/////////////////////////////////////////
//Display letter x 1
//6x8 pixcel/1 letter (16 x 8 letters in display)
//column=0-15 ; line=0-7
/////////////////////////////////////////
void display_one_letter (unsigned char z, unsigned char column, unsigned char line)
{
	unsigned char a;
	unsigned int letter;
	
	//	if((z<0x20)||(z>0x7f))z=0x20;
	if((z<0x20)||(z>0x80))z=0x20;
	letter = 6*(z-0x20);
	
	a = 6*(column&0x0f)+18;				//UG-9664HSWAG01 must add +18
	write_I2C_command(a&0x0f);			// Set low column
	write_I2C_command((((a&0xf0)>>4)+0x10));  	// Set high column
	a = (line&0x07);
	write_I2C_command(0xb0 + a);	                // Set line
	
	write_I2C_data(z_code[letter++]);
	write_I2C_data(z_code[letter++]);
	write_I2C_data(z_code[letter++]);
	write_I2C_data(z_code[letter++]);
	write_I2C_data(z_code[letter++]);
	write_I2C_data(z_code[letter++]);
	
	
	write_I2C_command(0xe3);	               // nop
}//End of display_one_letter


/*Function to display string on Oled*/
void lcd_Str(char colum, char line, char *s)
{
	while (*s)
	{
		display_one_letter (*s++, colum++, line);
		if(colum >= 16){
			line++;
			colum = 0;
			if(line >= 8)
				line = 0;
		}
	}
}//End of lcd_Str

//12x16 pixcel/1 letter (8 x 4 letters in display)
void lcd_Str_big(char colum, char line, char *s)
{
	while (*s)
	{
		
		display_big (*s++, colum++, line);
		if(colum >= 8){
			line++;
			colum = 0;
			if(line >= 4)
				line = 0;
		}
	}
}//End of lcd_Str_big
//////////////////////////////////////////////





/////////////////////////////////////////
//Display letter x 2
//12x16 pixcel/1 letter (8 x 4 letters in display)
//column=0-7 ; line=0-3
/////////////////////////////////////////
void display_big (unsigned char z, unsigned char column, unsigned char line)
{
	unsigned char a, b, c, d, e, f;
	unsigned int letter;
	
	//	if((z<0x20)||(z>0x7f))z=0x20;
	if((z<0x20)||(z>0x80))z=0x20;
	letter = 6*(z-0x20);
	
	a = 12*(column&0x07)+18;	//UG-9664HSWAG01 must add +18
	e = (a&0x0f);
	write_I2C_command(e);		// Set low column
	f = ((((a&0xf0)>>4)+0x10));
	write_I2C_command(f);		// Set high column
	d = 2*(line&0x03);
	
	for(c=0;c<6;c++)
	{
		a = z_code[letter++];
		write_I2C_command(0xb0 + d);	// Set line
		b = 0;
		if(a&0x01)b|=0x03;		// Detect letters
		if(a&0x02)b|=0x0c;
		if(a&0x04)b|=0x30;
		if(a&0x08)b|=0xc0;
		write_I2C_data(b);
		write_I2C_data(b);
		write_I2C_command(0xe3);	// nop
		write_I2C_command(e);		// Set low column
		write_I2C_command(f);		// Set high column
		write_I2C_command(0xb1 + d);	// Set line
		b = 0;
		if(a&0x10)b|=0x03;
		if(a&0x20)b|=0x0c;
		if(a&0x40)b|=0x30;
		if(a&0x80)b|=0xc0;
		write_I2C_data(b);
		write_I2C_data(b);
		write_I2C_command(0xe3);	// nop
		
		e = e + 2;			// Add culum +2
		if(e>0x0f)
		{
			e = e & 0x0f;
			f = f + 1;
		}
	}
}//End of display_big



//////////////////////////////////////////////////////////////////////
/////// Functions for display valiable //////////////////////////////
//////////////////////////////////////////////////////////////////////
//6x8 pixcel/1 letter (16 x 8 letters in display)
//x_posi:Column
//y_posi:Line
//x:Valiabale
//radix:10:Decimal, 16:Hex
//col:Can select Digit if setting 5 can show letters 0 to 99999
//d:0:Fill with "0",1:Not fill with "0"(Space)
//dot:Dot position
//////////////////////////////////////////////////////////////////////
void display_valiable( unsigned int x_posi,unsigned int y_posi,long x, int radix, int col, int d, int dot)
{
	int		i, j;
	char	ss[16], ds[16];
	
	// int -> Ascii
	//	itoa( x, ss, radix );
	//	ltoa( x, ss, radix );
	itoazz( x, ss, radix );
	
	ds[0] = 0x00;
	strcat( ds, ss );
	i = col - strlen( ss );
	// Add '0' or ' ' Supress
	if( i > 0 ) 
	{
		ds[0] = 0x00;
		for( j = 0; j < i; j++ ) {
			if( d == 0 ) {
				strcat( ds, "0" );
			} else {
				strcat( ds, " " );
			}
		}
		strcat( ds, ss );
	}
	// Add Dot
	i = strlen( ds );
	if ( ( i >= dot ) && ( dot > 0 ) ) 
	{
		strcpy( ss, ds );
		ds[0] = 0x00;
		mid( ds, ss, 1, i-dot );
		strcat( &ds[i-dot], "." );
		mid( &ds[i-dot+1], ss, i-dot+1, dot );
		if ( ( ( x == 0 ) || ( ds[i-dot-1] == ' ' ) ) && ( i >= 3 ) ) 
		{
			ds[i-dot-1] = '0';
		}
	}
	
	lcd_Str (x_posi, y_posi, ds);
}//End of display_valiable


/////////////////////////////////////////////////////////////////////
/////// Functions Letters control ///////////////////////////////////
/////////////////////////////////////////////////////////////////////
char *mid(char *ds, char *ss, int a, int b)
{
	register int	k, n, c;
	
	n = strlen(ss);
	if ( (0<a && a<=n) && (0<b) ) 
	{
		if ( (a+b-1) > n ) {
			c = n+1;
		}
		else {
			c = a+b;
		}
		for (k=a;k<c;k++) {
			ds[k-a] = ss[k-1];
		}
		ds[k-a] = '\0';
	}
	else {
		ds[0] = '\0';
	}
	return( ds );
}//End of *mid



char *itoazz( int value, char buff[], int radix )
{
	//static const char table[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	static const char table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *head = buff;
	char *tail = buff;
	char temp;
	
	// マイナス符号を文字列に変換
	if ( value < 0 )
	{
		*tail++ = '-';
		value = -value;
	}
	// 整数値を文字列に変換
	if ( value == 0 )
	{
		*tail++ = '0';
	}
	else for ( head = tail ; value != 0 ; value /= radix )
	{
		*tail++ = table[ value % radix ];
	}
	
	*tail = '\0';
	// 文字列の左右入れ替え
	for ( tail-- ; head < tail ; head++, tail-- )
	{
		temp = *head;
		*head = *tail;
		*tail = temp;
	}
	return buff;
}//End of *itoazz




/* This funtion issues a stop condition and therefore
* releases the bus
*/
void I2C_stop(I2C_TypeDef* I2Cx){
	// Send I2C1 STOP Condition 
	I2C_GenerateSTOP(I2Cx, ENABLE);
}



void all_clear(void)  //Clear all display
{
	int i;
	/*	Set curser to first
	*	The ssd 1305 is 132 x 64 lcd controll. We will clear all display ram on it.
	*	Number of byte to clear is (132*64)/8 = 1056 bytes. 
	*/
	write_I2C_command(0x00);	
	write_I2C_command(0x10);
	write_I2C_command(0xB0);	
	
	for(i=0;i<1056;i++)
	{
		write_I2C_data(0);	
	}
	
	//unsigned char a, b;
	//	for(a=0;a<8;a++)
	//	for(b=0;b<16;b++)display_one_letter(' ',b,a);
}//End of all_clear



void delay(unsigned int d)	//Delay 
{
	unsigned int i, j;
	for (i=0; i<d; i++)
		for (j=0; j<5000; j++);
}//End of delay
