#ifndef __PHYTIUM_GPIO_H__
#define __PHYTIUM_GPIO_H__

#include <stdint.h>

/* Phytium 2004/D2000 */
#define PINMUX_ADDR   0x28180000
#define GPIO_0_ADDR   0x28004000
#define GPIO_1_ADDR   0x28005000

/* Common */
#define GPIO_DIR_IN    0
#define GPIO_DIR_OUT   1

#define GPIO_CTRL_NUM  2
#define GPIO_PORT_NUM  2
#define GPIO_PIN_NUM   8

#define GPIO_NAME_LEN_MAX  8

typedef struct gpio {
	uint8_t ctrl;        /* gpio controller (0, 1) */
	char    port;        /* gpio port (a, b) */
	uint8_t pin;         /* gpio pin (0-7) */
} gpio_t;

typedef struct gpio_pinmux_cfg {
	gpio_t   gpio;
	uint64_t addr;       /* gpio pinmux address */
	uint8_t  bit_offset; /* gpio pinmux offset (2 bits) */
	uint8_t  val;        /* gpio pinmux value (2 bits) */
} gpio_pinmux_cfg_t;

typedef struct gpio_addr {
	gpio_t   gpio;
	uint64_t dir_addr;   /* gpio direction address */
	uint64_t in_addr;    /* gpio input address */
	uint64_t out_addr;   /* gpio output address */
} gpio_addr_t;

typedef struct gpio_desc {
	gpio_t   gpio;
	uint8_t  dir;        /* gpio direction */
	uint8_t  val;        /* gpio value */
	const gpio_pinmux_cfg_t *pinmux; /* gpio pinmux cfg info */
	const gpio_addr_t       *addr;   /* gpio addr info */
} gpio_desc_t;

/**
 *  gpioname: gpio description string
 *            it's format is "controller-port-pin"
 *            eg: 0-a-0, 1-b-7
 *  gpiodir : gpio direction
 *            0 input, 1 output
 */
gpio_desc_t *gpio_open(const char *gpioname, uint8_t gpiodir);
int gpio_close(gpio_desc_t *gpio);

int gpio_get(gpio_desc_t *gpio, uint8_t *val);
int gpio_set(gpio_desc_t *gpio, uint8_t val);

#endif /* __PHYTIUM_GPIO_H__ */
