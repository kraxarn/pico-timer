#pragma once

#include "pico/stdlib.h"

/**
 *
 */
enum xpm_status
{
	/**
	* XPM loaded successfully
	*/
	XPM_OK = 0,

	/**
	* The size of the XPM image is too large to fit on the display
	*/
	XPM_TOO_LARGE = 1,

	/**
	* The XPM image has more than 2 colors
	*/
	XPM_TOO_MANY_COLORS = 2,

	/**
	* The XPM contains more than one character per pixel
	*/
	XPM_TOO_MANY_CHARS_PER_PIXELS = 3,
};

enum xpm_status xpm_draw(char **data, uint8_t *buffer, uint8_t x, uint8_t y);

const char *xpm_status_to_string(enum xpm_status status);
