/**
   \file glcd_controllers.h
   \brief Functions specific to certain graphic LCD controllers.
          These are functions are defined in controllers/xxxx.c
		  
   \author Andy Gock
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

#ifndef GLCD_CONTROLLERS_H_
#define GLCD_CONTROLLERS_H_

/** \addtogroup Controllers
Controller specific functions.
Currently only the following controllers are supported:
- PCD8544 (Nokia 5110 LCD) SPI interface

The following is planned to be supported in the near future:
- KS0108 parallel interface
@{ */

/** Send command byte to LCD. */
void glcd_command(uint8_t c);

/** Send data byte to LCD. */
void glcd_data(uint8_t c);
	
/** Set contrast (Vop) from 0 to 127. This should be experimentally determined. Supported by PCD8544 only. */
void glcd_set_contrast(uint8_t val);

/** Power down the device. */
void glcd_power_down(void);

/** Power up the device. */
void glcd_power_up(void);

/** Set Y address of RAM (select bank). Device must be under basic instruction set mode before using this.
 *  \param y Y address of RAM (0 <= Y <= 5)
 */
void glcd_set_y_address(uint8_t y);

/** Set X address of RAM (column). Device must be under basic instruction set mode before using this.
 *  \param x X address of RAM (0 <= X <= 83)
 */
void glcd_set_x_address(uint8_t x);

/** Update the display within the specified bounding box. This physically writes data to the PCD8544's RAM. */
void glcd_write();

/** @}*/

#endif /* GLCD_CONTROLLERS_H_ */
