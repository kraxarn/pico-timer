#include <stdio.h>
#include <string.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "ssd1306.h"
#include "xpm.h"
#include "img/colon.xpm"
#include "img/num0.xpm"
#include "img/num1.xpm"
#include "img/num2.xpm"
#include "img/num3.xpm"
#include "img/num4.xpm"
#include "img/num5.xpm"
#include "img/num6.xpm"
#include "img/num7.xpm"
#include "img/num8.xpm"
#include "img/num9.xpm"
#include "img/space.xpm"

#define LED_DELAY_MS 3000

#define BUTTON_PIN 15

#define DIP1_PIN 11
#define DIP2_PIN 12
#define DIP3_PIN 13
#define DIP4_PIN 14

alarm_id_t current_timer = -1;

void pico_led_init()
{
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void button_init(const uint gpio)
{
	gpio_init(gpio);
	gpio_set_dir(gpio, GPIO_IN);
	gpio_pull_down(gpio);
}

unsigned char dip_value()
{
	return (gpio_get(DIP1_PIN) ? 1 : 0)
		| (gpio_get(DIP2_PIN) ? 2 : 0)
		| (gpio_get(DIP3_PIN) ? 4 : 0)
		| (gpio_get(DIP4_PIN) ? 8 : 0);
}

void pico_set_led(const bool led_on)
{
	printf("LED: %d\n", led_on);
	gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

void display_current_timeout()
{
	for (int i = 0; i < dip_value(); i++)
	{
		pico_set_led(true);
		sleep_ms(100);
		pico_set_led(false);
		sleep_ms(100);
	}

	sleep_ms(LED_DELAY_MS);
}

int64_t timer_callback(const alarm_id_t id, void *user_data)
{
	printf("Timer %d elapsed\n", id);

	*(alarm_id_t *)user_data = -1;
	return 0;
}

void start_timer()
{
	const int minutes = dip_value();
	const uint32_t milliseconds = minutes * 60 * 1000;
	printf("Starting timer for %d minute(s) (%d ms)\n", minutes, milliseconds);

	pico_set_led(true);
	sleep_ms(2000);
	pico_set_led(false);

	current_timer = add_alarm_in_ms(milliseconds, timer_callback, &current_timer, false);
}

bool is_timer_running()
{
	return current_timer >= 0;
}

void run_timer()
{
	if (!gpio_get(BUTTON_PIN))
	{
		return;
	}

	printf("Cancelling timer %d\n", current_timer);

	cancel_alarm(current_timer);
	current_timer = -1;

	pico_set_led(true);

	while (gpio_get(BUTTON_PIN))
	{
		sleep_ms(10);
	}

	pico_set_led(false);
}

void draw_time(uint8_t *buffer, const uint8_t x, const uint8_t y, const uint32_t time)
{
	char **numbers[10] = {
		num0,
		num1,
		num2,
		num3,
		num4,
		num5,
		num6,
		num7,
		num8,
		num9,
	};

	const uint32_t total_seconds = time / 1000;
	const uint32_t minutes = total_seconds / 60;
	const uint32_t seconds = total_seconds % 60;

	xpm_draw(numbers[minutes / 10], buffer, x + 16 * 0 + 8 * 0, y);
	xpm_draw(numbers[minutes % 10], buffer, x + 16 * 1 + 8 * 1, y);

	char **seperator = time % 1000 == 0 ? space : colon;
	xpm_draw(seperator, buffer, x + 16 * 2 + 8 * 1, y);

	xpm_draw(numbers[seconds / 10], buffer, x + 16 * 3 + 8 * 1, y);
	xpm_draw(numbers[seconds % 10], buffer, x + 16 * 4 + 8 * 2, y);
}

int main()
{
	stdio_usb_init();
	ssd1306_init_i2c();

	// Wait for LCD to initialize
	sleep_ms(2000);

	ssd1306_init();

	button_init(BUTTON_PIN);
	button_init(DIP1_PIN);
	button_init(DIP2_PIN);
	button_init(DIP3_PIN);
	button_init(DIP4_PIN);

	struct render_area area = {
		.start_col = 0,
		.end_col = SSD1306_WIDTH - 1,
		.start_page = 0,
		.end_page = SSD1306_NUM_PAGES - 1,
	};

	area.buffer_size = ssd1306_buffer_size(&area);

	uint8_t buffer[SSD1306_BUF_LEN] = {0};
	ssd1306_render(buffer, &area);

	uint32_t time = 99 * 60 * 1000;

	while (1)
	{
		if (time % 500 == 0)
		{
			draw_time(buffer, 16, 0, time);
			ssd1306_render(buffer, &area);
		}

		time--;
		sleep_ms(1);
	}

	printf("Ready!");

	while (true)
	{
		if (is_timer_running())
		{
			run_timer();
		}
		else
		{
			display_current_timeout();
		}

		if (!is_timer_running() && gpio_get(BUTTON_PIN))
		{
			start_timer();
		}
	}
}
