#ifndef __PHYTIUM_GPIO_H__
#define __PHYTIUM_GPIO_H__

#include <stdint.h>

#define GPIO_DIR_IN    0
#define GPIO_DIR_OUT   1

#define GPIO_DESC_LEN_MAX  8

typedef struct gpio {
	char desc[GPIO_DESC_LEN_MAX];
	uint8_t ctrl;
	uint8_t port;
	uint8_t pin;
	uint8_t dir;
} gpio_t;

/**
 *  *gpiodesc: gpio description string
 *             it's format is "controller-port-pin", eg: 1-b-7
 *  gpiodir  : gpio direction
 */
gpio_t *gpio_open(const char *gpiodesc, uint8_t gpiodir);
int gpio_close(gpio_t *gpio);

int gpio_get(gpio_t *gpio, uint8_t *val);
int gpio_set(gpio_t *gpio, uint8_t val);

#endif /* __PHYTIUM_GPIO_H__ */
