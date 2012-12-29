////////////////////////////////////////////
/////
////     AVR Component Tester
///       BrentBXR & MickM
// -----------------------------
// 
// LCD Routines ->
//     CID: 0x9938A4E
//     SID: 001
//
// $desc This file contains all the functions needed
//       to control a standard HDD chipset 2x16 char
//       LCD. 
//
//  ## This file is part of the original project and has ##
//  ##         been highly modified for our needs.       ##
//
 
#include <avr/io.h>
#include "lcd-routines.h"
#include "settings.h"
#include <util/delay.h>
#include <avr/eeprom.h>


void lcd_send(unsigned char data) {
  
  #ifdef LCD_SLOW_SEND
  _delay_us(500);
  #endif
  
  LCD_PORT = (LCD_PORT & 0xF0) | ((data >> 4) & 0x0F);
  _delay_us(5);
  
  lcd_enable();

  LCD_PORT = (LCD_PORT & 0xF0) | (data & 0x0F);
  _delay_us(5);
  
  lcd_enable();
  _delay_us(60);  
  
  LCD_PORT &= 0xF0;
}


void lcd_enable(void)
{
	LCD_PORT |= (1<<LCD_EN1);
    _delay_us(10);
   LCD_PORT &= ~(1<<LCD_EN1);
}
void lcd_clear(void)
{
   lcd_command(CLEAR_DISPLAY);
   _delay_ms(5);
}

/* eof */
