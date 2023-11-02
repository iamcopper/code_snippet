#include <stdint.h>
#include "phytium_gpio.h"

#define BASE_ADDR_PINMUX     0x28180000
#define BASE_ADDR_GPIO_CTRL0 0x28004000
#define BASE_ADDR_GPIO_CTRL1 0x28005000

#define GPIO_PORT_A_OUT      0x00
#define GPIO_PORT_A_DIR      0x04
#define GPIO_PORT_A_IN       0x08
#define GPIO_PORT_B_OUT      0x0C
#define GPIO_PORT_B_DIR      0x10
#define GPIO_PORT_B_IN       0x14

int gpio_init(gpio_t gpio, uint8_t dir)
{
	return 0;
}

int gpio_set(gpio_t gpio, uint8_t val)
{
	return 0;
}

int gpio_get(gpio_t gpio, uint8_t *val)
{
	return 0;
}
