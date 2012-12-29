/**
    \file glcd.c
	\author Andy Gock
	\brief Basic GLCD functions affecting bounding box manipulation,
	       clearing of screen and buffers, and basic scroll functions.
 */

/*
	Copyright (c) 2012, Andy Gock

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and/or other materials provided with the distribution.
		* Neither the name of the copyright holders nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include <stdio.h>
#include "glcd.h"

// display buffer (e.g 504 bytes for 48x84 LCD)
uint8_t glcd_buffer[GLCD_LCD_WIDTH * GLCD_LCD_HEIGHT / 8];
glcd_BoundingBox_t glcd_bbox;

// pointers to buffer and bounding box for current screen being updated
uint8_t *glcd_buffer_selected;
glcd_BoundingBox_t *glcd_bbox_selected;

void glcd_update_bbox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax)
{
	// keep and check bounding box within limits of LCD screen dimensions
	if (xmin < 0) {
		xmin = 0;
	}
	if (xmin > (GLCD_LCD_WIDTH-1)) {
		xmin = GLCD_LCD_WIDTH-1;
	}
	if (xmax < 0) {
		xmax = 0;
	}
	if (xmax > (GLCD_LCD_WIDTH-1)) {
		xmax = GLCD_LCD_WIDTH-1;
	}

	if (ymin < 0) {
		ymin = 0;
	}
	if (ymin > (GLCD_LCD_HEIGHT-1)) {
		ymin = GLCD_LCD_HEIGHT-1;
	}
	if (ymax < 0) {
		ymax = 0;
	}
	if (ymax > (GLCD_LCD_HEIGHT-1)) {
		ymax = GLCD_LCD_HEIGHT-1;
	}

	// update the bounding box size
	if (xmin < glcd_bbox_selected->x_min) {
		glcd_bbox_selected->x_min = xmin;
	}		
	if (xmax > glcd_bbox_selected->x_max) {
		glcd_bbox_selected->x_max = xmax;
	}
	if (ymin < glcd_bbox_selected->y_min) {
		glcd_bbox_selected->y_min = ymin;
	}		
	if (ymax > glcd_bbox_selected->y_max) {
		glcd_bbox_selected->y_max = ymax;
	}			
}

void glcd_reset_bbox()
{
	glcd_bbox_selected->x_min = GLCD_LCD_WIDTH - 1;
	glcd_bbox_selected->x_max = 0;
	glcd_bbox_selected->y_min = GLCD_LCD_HEIGHT -1;
	glcd_bbox_selected->y_max = 0;	
}

void glcd_clear(void) {
	memset(glcd_buffer_selected, 0x00, GLCD_LCD_WIDTH * GLCD_LCD_HEIGHT / 8);
	glcd_update_bbox(0,0,GLCD_LCD_WIDTH - 1,GLCD_LCD_HEIGHT - 1);
	glcd_write();
}

void glcd_clear_buffer(void) {
	memset(glcd_buffer_selected, 0x00, GLCD_LCD_WIDTH * GLCD_LCD_HEIGHT / 8);
	glcd_update_bbox(0,0,GLCD_LCD_WIDTH - 1,GLCD_LCD_HEIGHT - 1);
}

void glcd_select_screen(uint8_t *buffer, glcd_BoundingBox_t *bbox)
{
	glcd_buffer_selected = buffer;
	glcd_bbox_selected = bbox;
}

void glcd_scroll(int8_t x, int8_t y)
{
	for (uint8_t y=0; y<6; y++) {
		for (uint8_t x=0; x<84; x++) {
			
		}
	}
}

void glcd_scroll_line(void)
{
	for (uint8_t y=0; y<6; y++) {
		if (y <= 4) {
			// first 5 lines - banks 0 to 4
			memcpy(glcd_buffer_selected + y*84, glcd_buffer_selected + y*84 + 84, 84);
		} else {
			// last line - back 5 - clear it
			memset(glcd_buffer_selected + 5*84, 0x00, 84);
		}
	}
	glcd_update_bbox(0,0,GLCD_LCD_WIDTH - 1,GLCD_LCD_HEIGHT - 1);
}
