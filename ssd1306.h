#pragma once

#include "pico/stdlib.h"

#define SSD1306_HEIGHT              64
#define SSD1306_WIDTH               128

#define SSD1306_I2C_ADDR            _u(0x3D)

#define SSD1306_I2C_CLK             400

#define SSD1306_SET_MEM_MODE        _u(0x20)
#define SSD1306_SET_COL_ADDR        _u(0x21)
#define SSD1306_SET_PAGE_ADDR       _u(0x22)
#define SSD1306_SET_HORIZ_SCROLL    _u(0x26)
#define SSD1306_SET_SCROLL          _u(0x2E)

#define SSD1306_SET_DISP_START_LINE _u(0x40)

#define SSD1306_SET_CONTRAST        _u(0x81)
#define SSD1306_SET_CHARGE_PUMP     _u(0x8D)

#define SSD1306_SET_SEG_REMAP       _u(0xA0)
#define SSD1306_SET_ENTIRE_ON       _u(0xA4)
#define SSD1306_SET_ALL_ON          _u(0xA5)
#define SSD1306_SET_NORM_DISP       _u(0xA6)
#define SSD1306_SET_INV_DISP        _u(0xA7)
#define SSD1306_SET_MUX_RATIO       _u(0xA8)
#define SSD1306_SET_DISP            _u(0xAE)
#define SSD1306_SET_COM_OUT_DIR     _u(0xC0)
#define SSD1306_SET_COM_OUT_DIR_FLIP _u(0xC0)

#define SSD1306_SET_DISP_OFFSET     _u(0xD3)
#define SSD1306_SET_DISP_CLK_DIV    _u(0xD5)
#define SSD1306_SET_PRECHARGE       _u(0xD9)
#define SSD1306_SET_COM_PIN_CFG     _u(0xDA)
#define SSD1306_SET_VCOM_DESEL      _u(0xDB)

#define SSD1306_PAGE_HEIGHT         _u(8)
#define SSD1306_NUM_PAGES           (SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT)
#define SSD1306_BUF_LEN             (SSD1306_NUM_PAGES * SSD1306_WIDTH)

#define SSD1306_WRITE_MODE         _u(0xFE)
#define SSD1306_READ_MODE          _u(0xFF)

struct render_area
{
	uint8_t start_col;
	uint8_t end_col;
	uint8_t start_page;
	uint8_t end_page;

	int buffer_size;
};

int ssd1306_buffer_size(const struct render_area *area);
void ssd1306_init_i2c();
void ssd1306_write(uint8_t data);
void ssd1306_write_all(const uint8_t *data, int len);
void ssd1306_write_buffer(const uint8_t *buffer, int len);
void ssd1306_init();
void ssd1306_render(const uint8_t *buffer, const struct render_area *area);
void ssd1306_write_str(uint8_t *buffer, int16_t x, int16_t y, const char *str);