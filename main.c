#include <stdio.h>
#include "pico/stdlib.h"

#define LED_DELAY_MS 1

#define BUTTON_PIN 15

#define DIP1_PIN 11
#define DIP2_PIN 12
#define DIP3_PIN 13
#define DIP4_PIN 14

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
	gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

int main()
{
	stdio_usb_init();

	pico_led_init();

	button_init(BUTTON_PIN);
	button_init(DIP1_PIN);
	button_init(DIP2_PIN);
	button_init(DIP3_PIN);
	button_init(DIP4_PIN);

	while (true)
	{
		const bool button_value = gpio_get(BUTTON_PIN);
		pico_set_led(button_value);

		const bool dip1_value = gpio_get(DIP1_PIN);
		const bool dip2_value = gpio_get(DIP2_PIN);
		const bool dip3_value = gpio_get(DIP3_PIN);
		const bool dip4_value = gpio_get(DIP4_PIN);

		printf("btn0=%d dip1=%d dip2=%d dip3=%d dip4=%d dip=%d\n", button_value,
			dip1_value, dip2_value, dip3_value, dip4_value, dip_value());

		sleep_ms(LED_DELAY_MS);
	}
}
