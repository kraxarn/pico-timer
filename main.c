#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "ssd1306.h"

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

void lcd_init()
{
	i2c_init(i2c_default, 100 * 1000);
	gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
	gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
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

void lcd_send_cmd(const uint8_t cmd)
{
	const uint8_t buf[2] = {
		0x80,
		cmd,
	};

	i2c_write_blocking(i2c_default, SSD1306_I2C_ADDR, buf, 2, false);
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

int main()
{
	stdio_usb_init();

	pico_led_init();
	// lcd_init();

	button_init(BUTTON_PIN);
	button_init(DIP1_PIN);
	button_init(DIP2_PIN);
	button_init(DIP3_PIN);
	button_init(DIP4_PIN);

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
