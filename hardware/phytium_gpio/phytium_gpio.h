#ifndef __PHYTIUM_GPIO_H__
#define __PHYTIUM_GPIO_H__

#include <stdint.h>
#include "devmem.h"

#define GPIO_DIR_IN    0
#define GPIO_DIR_OUT   1

#define GPIO_CTRL_NUM  2  /* 0, 1 */
#define GPIO_PORT_NUM  2  /* a, b */
#define GPIO_PIN_NUM   8  /* 0 ~ 7 */

#define GPIO_NAME_LEN_MAX  8
#define FUNC_NAME_LEN_MAX  8

typedef struct gpio {
	uint8_t ctrl;        /* gpio controller number (0, 1) */
	char    port;        /* gpio port number (a, b) */
	uint8_t pin;         /* gpio pin number (0-7) */
} gpio_t;

typedef struct gpio_pinmux {
	gpio_t   gpio;
	devmem_t *devmem;    /* gpio pinmux devmem handle */
	uint8_t  width;      /* gpio pinmux reg width */
	uint64_t baseaddr;   /* gpio pinmux base address */
	uint8_t  offset;     /* gpio pinmux address offset */
	uint8_t  h_bit;      /* gpio pinmux bit offset (2 bits) */
	uint8_t  set_val;    /* gpio pinmux value need to set (2 bits) */
	uint8_t  val;        /* gpio pinmux current value */
} gpio_pinmux_t;

typedef struct gpio_ctrl {
	gpio_t   gpio;
	devmem_t *devmem;    /* gpio devmem handle */
	uint8_t  width;      /* gpio reg width */
	uint64_t baseaddr;   /* gpio base address */
	uint8_t  dir_offset; /* gpio direction address offset */
	uint8_t  in_offset;  /* gpio input address offset */
	uint8_t  out_offset; /* gpio output address offset */
} gpio_ctrl_t;

typedef struct gpio_desc {
	gpio_t   gpio;
	uint8_t  dir;        /* gpio current direction */
	uint8_t  val;        /* gpio current value */
	gpio_pinmux_t  *pinmux; /* gpio pinmux control */
	gpio_ctrl_t    *ctrl;   /* gpio ctroller control */
} gpio_desc_t;

/**
 *  gpioname: gpio description string
 *            it's format is "controller-port-pin"
 *            eg: 0-a-0, 1-b-7
 *  gpiodir : gpio direction
 *            0 input, 1 output
 */
gpio_desc_t *gpio_open(const char *gpioname);
int gpio_close(gpio_desc_t *gpio_desc);
int gpio_set_dir(gpio_desc_t *gpio_desc, uint8_t dir);
int gpio_set_val(gpio_desc_t *gpio_desc, uint8_t val);
int gpio_get_dir(gpio_desc_t *gpio_desc, uint8_t *dir);
int gpio_get_val(gpio_desc_t *gpio_desc, uint8_t *val);
int gpio_get_dir_val(gpio_desc_t *gpio_desc, uint8_t *dir, uint8_t *val);

int gpio_set_dir2(const char *gpioname, uint8_t dir);
int gpio_set_val2(const char *gpioname, uint8_t val);
int gpio_get_dir2(const char *gpioname, uint8_t *dir);
int gpio_get_val2(const char *gpioname, uint8_t *val);
int gpio_get_dir_val2(const char *gpioname, uint8_t *dir, uint8_t *val);

#endif /* __PHYTIUM_GPIO_H__ */
