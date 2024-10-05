#include "ssd1306.h"
#include "ssd1306_font.h"

#include <stdlib.h>
#include <string.h>

#include "hardware/i2c.h"

uint8_t display_buffer[SSD1306_BUF_LEN];

int ssd1306_buffer_size(const struct render_area *area)
{
	return (area->end_col - area->start_col + 1) * (area->end_page - area->start_page + 1);
}

void ssd1306_init_i2c()
{
	i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
	gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
	gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

void ssd1306_write(const uint8_t data)
{
	const uint8_t buffer[2] = {
		0x80,
		data,
	};

	i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buffer, 2, false);
}

void ssd1306_write_all(const uint8_t *data, const int len)
{
	for (int i = 0; i < len; i++)
	{
		ssd1306_write(data[i]);
	}
}

void ssd1306_write_buffer(const uint8_t *buffer, const int len)
{
	uint8_t *temp = malloc(len + 1);

	temp[0] = 0x40;
	memcpy(temp + 1, buffer, len);

	i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, temp, len + 1, false);

	free(temp);
}

void ssd1306_init()
{
	const uint8_t cmds[] = {
		SSD1306_SET_DISP, // set display off
		/* memory mapping */
		SSD1306_SET_MEM_MODE, // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
		0x00,                 // horizontal addressing mode
		/* resolution and layout */
		SSD1306_SET_DISP_START_LINE,    // set display start line to 0
		SSD1306_SET_SEG_REMAP | 0x01,   // set segment re-map, column address 127 is mapped to SEG0
		SSD1306_SET_MUX_RATIO,          // set multiplex ratio
		SSD1306_HEIGHT - 1,             // Display height - 1
		SSD1306_SET_COM_OUT_DIR | 0x08, // set COM (common) output scan direction. Scan from bottom up, COM[N-1] to COM0
		SSD1306_SET_DISP_OFFSET,        // set display offset
		0x00,                           // no offset
		SSD1306_SET_COM_PIN_CFG,        // set COM (common) pins hardware configuration. Board specific magic number.
		// 0x02 Works for 128x32, 0x12 Possibly works for 128x64. Other options 0x22, 0x32
		0x12,
		/* timing and driving scheme */
		SSD1306_SET_DISP_CLK_DIV, // set display clock divide ratio
		0x80,                     // div ratio of 1, standard freq
		SSD1306_SET_PRECHARGE,    // set pre-charge period
		0xF1,                     // Vcc internally generated on our board
		SSD1306_SET_VCOM_DESEL,   // set VCOMH deselect level
		0x30,                     // 0.83xVcc
		/* display */
		SSD1306_SET_CONTRAST, // set contrast control
		0xFF,
		SSD1306_SET_ENTIRE_ON,   // set entire display on to follow RAM content
		SSD1306_SET_NORM_DISP,   // set normal (not inverted) display
		SSD1306_SET_CHARGE_PUMP, // set charge pump
		0x14,                    // Vcc internally generated on our board
		SSD1306_SET_SCROLL | 0x00,
		// deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
		SSD1306_SET_DISP | 0x01, // turn display on
	};

	ssd1306_write_all(cmds, count_of(cmds));
}

void ssd1306_render(const uint8_t *buffer, const struct render_area *area)
{
	const uint8_t cmds[] = {
		SSD1306_SET_COL_ADDR,
		area->start_col,
		area->end_col,
		SSD1306_SET_PAGE_ADDR,
		area->start_page,
		area->end_page,
	};

	ssd1306_write_all(cmds, count_of(cmds));
	ssd1306_write_buffer(buffer, area->buffer_size);
}

int get_font_index(const char ch)
{
	if (ch >= 'A' && ch <= 'Z')
	{
		return ch - 'A' + 1;
	}

	if (ch >= '0' && ch <= '9')
	{
		return ch - '0' + 27;
	}

	return 0;
}

void ssd1306_write_char(uint8_t *buffer, const int16_t x, int16_t y, const char ch)
{
	if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
	{
		return;
	}

	// For the moment, only write on Y row boundaries (every 8 vertical pixels)
	y = y / 8;

	const int index = get_font_index(ch);
	int fb_index = y * 128 + x;

	for (int i = 0; i < 8; i++)
	{
		buffer[fb_index++] = font[index * 8 + i];
	}
}

void ssd1306_write_str(uint8_t *buffer, int16_t x, const int16_t y, const char *str)
{
	if (x > SSD1306_WIDTH - 8 || y > SSD1306_HEIGHT - 8)
	{
		return;
	}

	while (*str)
	{
		ssd1306_write_char(buffer, x, y, *str++);
		x += 8;
	}
}
