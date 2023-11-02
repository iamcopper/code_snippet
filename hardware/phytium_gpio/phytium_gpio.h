#ifndef __PHYTIUM_GPIO_H__
#define __PHYTIUM_GPIO_H__

/*
 * Phytium 2004 and D2000 GPIO Ctrl
 */

typedef struct gpio {
	uint8_t ctrl;
	uint8_t port;
	uint8_t pin;
} gpio_t;

int gpio_init(gpio_t gpio, uint8_t dir);
int gpio_set(gpio_t gpio, uint8_t val);
int gpio_get(gpio_t gpio, uint8_t *val);

#endif
