/*oledApp.h*/
#ifndef _OLED_APP_H
#define _OLED_APP_H

#define OLED_I2C_ADDRESS	0x78	/* 0x3C  SSD1305 I2C address 0b0011 1100*/

#endif


void Oled_SSD1305_Init();

void display_one_letter (unsigned char z, unsigned char column, unsigned char line);
void display_big (unsigned char z, unsigned char column, unsigned char line);
void lcd_Str(char colum, char line, char *s);
void lcd_Str_big(char colum, char line, char *s);
void display_valiable( unsigned int x_posi,unsigned int y_posi,long x, int radix, int col, int d, int dot);