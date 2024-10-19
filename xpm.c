#include "xpm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssd1306.h"
#include "pico/stdlib.h"

struct xpm_values
{
	uint8_t columns;
	uint8_t rows;
	uint8_t colors;
	uint8_t chars_per_pixel;
};

struct xpm_color
{
	char key;
	uint8_t value;
};

struct xpm_values xpm_parse_values(const char *values)
{
	struct xpm_values result = {0};

	char *end1 = strchr(values, ' ');
	result.columns = strtol(values, &end1, 10);

	char *end2 = strchr(end1, ' ');
	result.rows = strtol(end1 + 1, &end2, 10);

	char *end3 = strchr(end2, ' ');
	result.colors = strtol(end2 + 1, &end3, 10);

	char *end4 = strchr(end3, ' ');
	result.chars_per_pixel = strtol(end3 + 1, &end4, 10);

	return result;
}

void xpm_parse_colors(const struct xpm_values *values, char **data, struct xpm_color *colors)
{
	for (uint8_t i = 0; i < values->colors; i++)
	{
		colors[i].key = data[i + 1][0];
		colors[i].value = strcmp(&data[i + 1][4], "black") == 0 ? 0x00 : 0xff;
	}
}

enum xpm_status xpm_draw(char **data, uint8_t *buffer, const uint8_t x, const uint8_t y)
{
	const struct xpm_values values = xpm_parse_values(data[0]);

	if (values.columns + x > SSD1306_WIDTH || values.rows + y > SSD1306_HEIGHT)
	{
		return XPM_TOO_LARGE;
	}

	if (values.colors > 2)
	{
		return XPM_TOO_MANY_COLORS;
	}

	if (values.chars_per_pixel > 1)
	{
		return XPM_TOO_MANY_CHARS_PER_PIXELS;
	}

	struct xpm_color colors[2] = {0};
	xpm_parse_colors(&values, data, colors);

	char **pixels = data + values.colors + 1;

	for (uint8_t px_y = 0; px_y < values.rows; px_y += 8)
	{
		for (uint8_t px_x = 0; px_x < values.columns; px_x++)
		{
			const uint8_t val = (pixels[px_y][px_x] == ' ' ? 0 : 1)
				| (pixels[px_y + 1][px_x] == ' ' ? 0 : 2)
				| (pixels[px_y + 2][px_x] == ' ' ? 0 : 4)
				| (pixels[px_y + 3][px_x] == ' ' ? 0 : 8)
				| (pixels[px_y + 4][px_x] == ' ' ? 0 : 16)
				| (pixels[px_y + 5][px_x] == ' ' ? 0 : 32)
				| (pixels[px_y + 6][px_x] == ' ' ? 0 : 64)
				| (pixels[px_y + 7][px_x] == ' ' ? 0 : 128);

			buffer[(y + px_y) / 8 * 128 + x + px_x] = val;
		}
	}

	return XPM_OK;
}

const char *xpm_status_to_string(const enum xpm_status status)
{
	switch (status)
	{
		case XPM_OK:
			return "ok";

		case XPM_TOO_LARGE:
			return "too large";

		case XPM_TOO_MANY_COLORS:
			return "too many colors";

		case XPM_TOO_MANY_CHARS_PER_PIXELS:
			return "too many chars per pixel";
	}

	return "unknown status";
}
