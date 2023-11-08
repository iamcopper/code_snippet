#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "phytium_gpio.h"
#include "devmem.h"

/* Phytium 2004/D2000 */
#define PINMUX_ADDR   0x28180000
#define GPIO_0_ADDR   0x28004000
#define GPIO_1_ADDR   0x28005000

/* GPIO CMP Flags */
#define GPIO_CMP_ALL       (GPIO_CMP_CTRL | GPIO_CMP_PORT | GPIO_CMP_PIN)
#define GPIO_CMP_CTRL_PORT (GPIO_CMP_CTRL | GPIO_CMP_PORT)
#define GPIO_CMP_CTRL      (1 << 2)
#define GPIO_CMP_PORT      (1 << 1)
#define GPIO_CMP_PIN       (1 << 0)

#define ARRAY_SIZE(array) \
	sizeof(array)/(sizeof(array[0]))

typedef struct gpio_pin {
	uint8_t ctrl;        /* gpio controller (0, 1) */
	char    port;        /* gpio port (a, b) */
	uint8_t pin;         /* gpio pin (0-7) */
} gpio_pin_t;

typedef struct gpio_pinmux {
	gpio_pin_t  gpio_pin;
	uint64_t    addr;       /* gpio pinmux address */
	uint8_t     h_bit;      /* gpio pinmux hight bit offset (2 bits) */
	uint8_t     set_val;    /* gpio pinmux set value (2 bits) */
} gpio_pinmux_t;

typedef struct gpio_ctrl {
	gpio_pin_t   gpio_pin;
	uint64_t    dir_addr;   /* gpio direction address */
	uint64_t    in_addr;    /* gpio input address */
	uint64_t    out_addr;   /* gpio output address */
} gpio_ctrl_t;

typedef struct gpio_desc {
	gpio_pin_t   gpio_pin;
	const gpio_pinmux_t *pinmux; /* gpio pinmux info */
	const gpio_ctrl_t   *ctrl;   /* gpio ctrl info */
} gpio_desc_t;

const static gpio_pinmux_t gpio_pinmux[] = {
	{{0, 'a', 0}, PINMUX_ADDR + 0x200, 25, 0x1},
	{{0, 'a', 1}, PINMUX_ADDR + 0x200, 21, 0x1},
	{{0, 'a', 2}, PINMUX_ADDR + 0x200, 17, 0x1},
	{{0, 'a', 3}, PINMUX_ADDR + 0x200, 13, 0x1},
	{{0, 'a', 4}, PINMUX_ADDR + 0x200,  9, 0x1},
	{{0, 'a', 5}, PINMUX_ADDR + 0x200,  5, 0x1},
	{{0, 'a', 6}, PINMUX_ADDR + 0x200,  1, 0x1},
	{{0, 'a', 7}, PINMUX_ADDR + 0x204, 29, 0x1},
	{{0, 'b', 0}, PINMUX_ADDR + 0x228, 25, 0x2},
	{{0, 'b', 1}, PINMUX_ADDR + 0x228, 21, 0x2},
	{{0, 'b', 2}, PINMUX_ADDR + 0x228, 17, 0x2},
	{{0, 'b', 3}, PINMUX_ADDR + 0x22C, 21, 0x2},
	{{0, 'b', 4}, PINMUX_ADDR + 0x22C, 17, 0x2},
	{{0, 'b', 5}, PINMUX_ADDR + 0x210,  1, 0x2},
	{{0, 'b', 6}, PINMUX_ADDR + 0x214,  8, 0x2},
	{{0, 'b', 7}, PINMUX_ADDR + 0x214,  5, 0x2},
	{{1, 'a', 0}, PINMUX_ADDR + 0x228, 13, 0x2},
	{{1, 'a', 1}, PINMUX_ADDR + 0x228,  8, 0x2},
	{{1, 'a', 2}, PINMUX_ADDR + 0x228,  5, 0x2},
	{{1, 'a', 3}, PINMUX_ADDR + 0x218, 13, 0x1},
	{{1, 'a', 4}, PINMUX_ADDR + 0x218,  9, 0x1},
	{{1, 'a', 5}, PINMUX_ADDR + 0x208, 17, 0x1},
	{{1, 'a', 6}, PINMUX_ADDR + 0x208, 13, 0x1},
	{{1, 'a', 7}, PINMUX_ADDR + 0x208,  9, 0x1},
	{{1, 'b', 0}, PINMUX_ADDR + 0x208,  5, 0x1},
	{{1, 'b', 1}, PINMUX_ADDR + 0x208,  1, 0x1},
	{{1, 'b', 2}, PINMUX_ADDR + 0x20C, 29, 0x1},
	{{1, 'b', 3}, PINMUX_ADDR + 0x20C, 25, 0x1},
	{{1, 'b', 4}, PINMUX_ADDR + 0x20C, 21, 0x1},
	{{1, 'b', 5}, PINMUX_ADDR + 0x20C, 17, 0x1},
	{{1, 'b', 6}, PINMUX_ADDR + 0x20C, 13, 0x1},
	{{1, 'b', 7}, PINMUX_ADDR + 0x200, 21, 0x1},
};

const static gpio_ctrl_t gpio_ctrl[] = {
	{{0, 'a', 0}, GPIO_0_ADDR + 0x04, GPIO_0_ADDR + 0x08, GPIO_0_ADDR + 0x00},
	{{0, 'b', 0}, GPIO_0_ADDR + 0x10, GPIO_0_ADDR + 0x14, GPIO_0_ADDR + 0x0C},
	{{1, 'a', 0}, GPIO_1_ADDR + 0x04, GPIO_1_ADDR + 0x08, GPIO_1_ADDR + 0x00},
	{{1, 'b', 0}, GPIO_1_ADDR + 0x10, GPIO_1_ADDR + 0x14, GPIO_1_ADDR + 0x0C},
};

static int _gpio_pin_cmp(const gpio_pin_t *p1, const gpio_pin_t *p2, uint8_t flag)
{
	if ((flag & GPIO_CMP_CTRL) && (p1->ctrl != p2->ctrl))
		return (p1->ctrl - p2->ctrl);

	if ((flag & GPIO_CMP_PORT) && (p1->port != p2->port))
		return (p1->port - p2->port);

	if (flag & GPIO_CMP_PIN)
		return (p1->pin - p2->pin);

	return 0;
}

static int _gpio_desc_init(gpio_desc_t *gpio_desc, const char *gpioname)
{
	char *str = NULL;
	char strtmp[GPIO_NAME_LEN_MAX] = {0};
	int i;

	if (!gpio_desc || !gpioname)
		return -1;

	strncpy(strtmp, gpioname, GPIO_NAME_LEN_MAX);

	/* get gpio controller */
	str = strtok(strtmp, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO CTRL get error\n");
		return -1;
	}
	gpio_desc->gpio_pin.ctrl = strtoul(str, NULL, 0);
	if (errno || gpio_desc->gpio_pin.ctrl >= GPIO_CTRL_NUM) {
		fprintf(stderr, "[ERROR] GPIO CTRL format or range error\n");
		return -1;
	}

	/* get gpio port */
	str = strtok(str, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO PORT get error\n");
		return -1;
	}
	gpio_desc->gpio_pin.port = tolower(str[0]);
	if (gpio_desc->gpio_pin.port < 'a' || gpio_desc->gpio_pin.port > 'b') {
		fprintf(stderr, "[ERROR] GPIO PORT range error: %c\n", gpio_desc->gpio_pin.port);
		return -1;
	}

	/* get gpio pin */
	str = strtok(str, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO PIN get error\n");
		return -1;
	}
	gpio_desc->gpio_pin.pin = strtoul(str, NULL, 0);
	if (errno || gpio_desc->gpio_pin.pin >= GPIO_PIN_NUM) {
		fprintf(stderr, "[ERROR] GPIO PIN format or range error: %d\n", gpio_desc->gpio_pin.pin);
		return -1;
	}

	/* get pinmux cfg */
	for (i = 0; i < ARRAY_SIZE(gpio_pinmux); i++) {
		if (0 == _gpio_pin_cmp(&gpio_desc->gpio_pin, &gpio_pinmux[i].gpio_pin, GPIO_CMP_ALL)) {
			gpio_desc->pinmux = &gpio_pinmux[i];
			break;
		}
	}
	if (!gpio_desc->pinmux) {
		fprintf(stderr, "[ERROR] gpio_pinmux not found\n");
		return -1;
	}

	/* get gpio addr info */
	for (i = 0; i < ARRAY_SIZE(gpio_ctrl); i++) {
		if (0 == _gpio_pin_cmp(&gpio_desc->gpio_pin, &gpio_ctrl[i].gpio_pin, GPIO_CMP_CTRL_PORT)) {
			gpio_desc->ctrl = &gpio_ctrl[i];
			break;
		}
	}
	if (!gpio_desc->ctrl) {
		fprintf(stderr, "[ERROR] gpio_ctrl not found\n");
		return -1;
	}

	return 0;
}

static int _gpio_set_pinmux_reg(const gpio_desc_t *gpio_desc)
{
	int ret;
	uint32_t devmem_val;

	if (!gpio_desc)
		return -1;

	ret = devmem_read2((void *)gpio_desc->pinmux->addr, 0, 32, &devmem_val);
	if (ret < 0)
		return -1;

	/* phytium gpio pinmux: 2 bits */
	devmem_val &= ~(0x3 << gpio_desc->pinmux->h_bit);
	devmem_val |= (gpio_desc->pinmux->set_val << gpio_desc->pinmux->h_bit);

	ret = devmem_write2((void *)gpio_desc->pinmux->addr, 0, 32, devmem_val);
	if (ret < 0)
		return -1;

	return 0;
}

static int _gpio_set_dir_reg(const gpio_desc_t *gpio_desc, uint8_t dir)
{
	if (!gpio_desc)
		return -1;

	if (dir == GPIO_DIR_IN)
		return devmem_clr_bit((void *)gpio_desc->ctrl->dir_addr, 0, 32, gpio_desc->gpio_pin.pin);

	return devmem_set_bit2((void *)gpio_desc->ctrl->dir_addr, 0, 32, gpio_desc->gpio_pin.pin);
}

static int _gpio_get_dir_reg(const gpio_desc_t *gpio_desc, uint8_t *dir)
{
	int ret;

	if (!gpio_desc)
		return -1;

	ret = devmem_get_bit2((void *)gpio_desc->ctrl->dir_addr, 0, 32, gpio_desc->gpio_pin.pin);
	if (ret < 0)
		return -1;

	*dir = (uint8_t)ret & 0x1;
	return 0;
}

static int _gpio_set_val_reg(gpio_desc_t *gpio_desc, uint8_t val)
{
	if (!gpio_desc)
		return -1;

	if (val == 0)
		return devmem_clr_bit2((void *)gpio_desc->ctrl->out_addr, 0, 32, gpio_desc->gpio_pin.pin);

	return devmem_set_bit2((void *)gpio_desc->ctrl->out_addr, 0, 32, gpio_desc->gpio_pin.pin);
}

static int _gpio_get_val_reg(gpio_desc_t *gpio_desc, uint8_t *val)
{
	int ret;
	void *gpio_addr;
	uint8_t dir;

	if (!gpio_desc || !val)
		return -1;

	ret = _gpio_get_dir_reg(gpio_desc, &dir);
	if (ret < 0)
		return -1;

	if (dir == GPIO_DIR_IN)
		gpio_addr = (void *)gpio_desc->ctrl->in_addr;
	else
		gpio_addr = (void *)gpio_desc->ctrl->out_addr;

	ret = devmem_get_bit2(gpio_addr, 0, 32, gpio_desc->gpio_pin.pin);
	if (ret < 0)
		return -1;

	*val= (uint8_t)ret & 0x1;
	return 0;
}

int gpio_init(const char *gpioname, uint8_t gpiodir)
{
	gpio_desc_t gpio_desc;
	int ret;

	ret = _gpio_desc_init(&gpio_desc, gpioname);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO desc init failed.");
		return -1;
	}

	ret = _gpio_set_pinmux_reg(&gpio_desc);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO Set pinmux reg failed.");
		return ret;
	}

	ret = _gpio_set_dir_reg(&gpio_desc, gpiodir);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO Set dir reg failed.");
		return ret;
	}

	return 0;
}

int gpio_set(const char *gpioname, uint8_t val)
{
	gpio_desc_t gpio_desc;
	int ret;

	ret = _gpio_desc_init(&gpio_desc, gpioname);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO desc init failed.");
		return -1;
	}

	ret = _gpio_set_val_reg(&gpio_desc, val);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO set val failed.");
		return -1;
	}

	return 0;
}

int gpio_get(const char *gpioname, uint8_t *val)
{
	gpio_desc_t gpio_desc;
	int ret;

	ret = _gpio_desc_init(&gpio_desc, gpioname);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO desc init failed.");
		return -1;
	}

	ret = _gpio_get_val_reg(&gpio_desc, val);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO get val failed.");
		return -1;
	}

	return 0;
}
