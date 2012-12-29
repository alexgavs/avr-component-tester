/** Sample program for PCD8544 library.
 * \file main.c
 *
 */ 

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "library/glcd/glcd.h"

#define LCD_BACKLIGHT_PORT PORTB
#define LCD_BACKLIGHT_PIN  6

int main()
{
	// dataflash memory share the SPI lines, set their SS pins high
	// to disable them (PE0 and PE1)
	DDRE |= _BV(0) | _BV(1);
	PORTE |= _BV(0) | _BV(1);

	// set LEDs to OFF
	sbi(DDRD,4);
	cbi(PORTD,4);
	cbi(PORTD,5);
	
	// enable backlight pin (really should set this with PWM)	
	sbi(DDR(LCD_BACKLIGHT_PORT),LCD_BACKLIGHT_PIN);
	sbi(LCD_BACKLIGHT_PORT,LCD_BACKLIGHT_PIN);

	// do some stuff on the LCD
	glcd_init();
	glcd_set_contrast(50);
	glcd_clear();

	//glcd_test_circles();
	glcd_test_counter_and_graph();
	//glcd_test_text_up_down();
	
	while(1);
	
	return 1;
}


