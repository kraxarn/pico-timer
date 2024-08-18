#include <stdio.h>
#include "pico/stdlib.h"

#define LED_DELAY_MS 1

#define BUTTON_PIN 14

void pico_led_init()
{
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void button_init()
{
	gpio_init(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);
	gpio_pull_down(BUTTON_PIN);
}

void pico_set_led(const bool led_on)
{
	gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

int main(void)
{
	stdio_usb_init();

	pico_led_init();
	button_init();

	while (true)
	{
		const bool button_value = gpio_get(BUTTON_PIN);
		pico_set_led(button_value);

		printf("button_value: %d\n", button_value);

		sleep_ms(LED_DELAY_MS);
	}
}
