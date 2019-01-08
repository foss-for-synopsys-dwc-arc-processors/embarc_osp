/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

#include "embARC.h"
#include "embARC_debug.h"

/* middleware level*/
#include "u8g.h"

u8g_t u8g;

void u8g_prepare(void) {
	u8g_SetFont(&u8g, u8g_font_6x10);		/* set the current font and reset the font reference position to "Baseline" */
	u8g_SetFontRefHeightExtendedText(&u8g);		/* define the calculation method for the ascent and descent of the current font */
	u8g_SetDefaultForegroundColor(&u8g);		/* assign one of the default colors as current color index */
	u8g_SetFontPosTop(&u8g);			/* set the reference position for the character and string draw procedure */
}

/** first page in OLED */
void u8g_box_frame(uint8_t a) {
	u8g_DrawStr(&u8g, 0, 0, "embARC");		/* draws a string at the specified x/y position */
	u8g_DrawBox(&u8g, 5,10,20,10);			/* draw a box (filled frame), starting at x/y position (upper left edge) */
	u8g_DrawBox(&u8g, 10+a,15,30,7);
	u8g_DrawStr(&u8g, 0, 30, "drawFrame");
	u8g_DrawFrame(&u8g, 5,10+30,20,10);		/* draw a frame, starting at x/y position (upper left edge) */
	u8g_DrawFrame(&u8g, 10+a,15+30,30,7);
}

/** second page in OLED */
void u8g_string(uint8_t a) {
	u8g_DrawStr(&u8g, 30+a,31, " 0");
	u8g_DrawStr90(&u8g, 30,31+a, " 90");		/* rotate string output by 90 degree */
	u8g_DrawStr180(&u8g, 30-a,31, " 180");		/* rotate string output by 180 degree */
	u8g_DrawStr270(&u8g, 30,31-a, " 270");		/* rotate string output by 270 degree */
}

/** third page in OLED */
void u8g_line(uint8_t a) {
	u8g_DrawStr(&u8g, 0, 0, "drawLine");
	u8g_DrawLine(&u8g, 7+a, 10, 40, 55);		/* draw a line from (x1, y1) to (x2, y2) */
	u8g_DrawLine(&u8g, 7+a*2, 10, 60, 55);
	u8g_DrawLine(&u8g, 7+a*3, 10, 80, 55);
	u8g_DrawLine(&u8g, 7+a*4, 10, 100, 55);
}

/** forth page in OLED */
void u8g_ascii_1(void) {
	char s[2] = " ";
	uint8_t x, y;
	u8g_DrawStr(&u8g, 0, 0, "ASCII page 1");
	for( y = 0; y < 6; y++ ) {
		for( x = 0; x < 16; x++ ) {
			s[0] = y*16 + x + 32;
			u8g_DrawStr(&u8g, x*7, y*10+10, s);
		}
	}
}

/** fifth page in OLED */
void u8g_ascii_2(void) {
	char s[2] = " ";
	uint8_t x, y;
	u8g_DrawStr(&u8g, 0, 0, "ASCII page 2");
	for( y = 0; y < 6; y++ ) {
		for( x = 0; x < 16; x++ ) {
			s[0] = y*16 + x + 160;
			u8g_DrawStr(&u8g, x*7, y*10+10, s);
		}
	}
}

uint8_t draw_state = 0;

/** draw five pages in OLED */
void draw(void) {
	u8g_prepare();
	switch(draw_state >> 3) {
		case 0: u8g_box_frame(draw_state&7); break;
		case 1: u8g_string(draw_state&7); break;
		case 2: u8g_line(draw_state&7); break;
		case 3: u8g_ascii_1(); break;
		case 4: u8g_ascii_2(); break;
	}
}


/** main entry for running ntshell */
int main(void)
{
	u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_2x_i2c, U8G_COM_SSD_I2C); /* create a new interface to a graphics display */

	EMBARC_PRINTF("u8glib\r\n");

	EMBARC_PRINTF("Display Width: %u, Display Height: %u\r\n" , u8g_GetWidth(&u8g), u8g_GetHeight(&u8g));

	u8g_Begin(&u8g); /* reset display and put it into default state */

	while(1) {
		/* picture loop */
		u8g_FirstPage(&u8g); /* marks the beginning of the picture loop; it cannot be used inside the picture loop */
		do {
			draw();
		} while (u8g_NextPage(&u8g)); /* marks the end of the body of the picture loop */

		draw_state++;
		if (draw_state >= 5*8) {	/* refresh 8 times for every "draw_state" */
			draw_state = 0;
		}
	}

	return E_SYS;
}

