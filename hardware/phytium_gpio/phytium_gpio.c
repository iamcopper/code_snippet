#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "phytium_gpio.h"
#include "devmem.h"

#define ARRAY_SIZE(array) \
	sizeof(array)/(sizeof(array[0]))

const static gpio_pinmux_cfg_t gpio_pinmux_cfg[] = {
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

const static gpio_addr_t gpio_addr[] = {
	{{0, 'a', 0}, GPIO_0_ADDR + 0x04, GPIO_0_ADDR + 0x08, GPIO_0_ADDR + 0x00},
	{{0, 'b', 0}, GPIO_0_ADDR + 0x10, GPIO_0_ADDR + 0x14, GPIO_0_ADDR + 0x0C},
	{{1, 'a', 0}, GPIO_1_ADDR + 0x04, GPIO_1_ADDR + 0x08, GPIO_1_ADDR + 0x00},
	{{1, 'b', 0}, GPIO_1_ADDR + 0x10, GPIO_1_ADDR + 0x14, GPIO_1_ADDR + 0x0C},
};

static int _gpio_set_pinmux_reg(const gpio_desc_t *gpio_desc)
{
	int devmem_fd, ret;
	uint32_t devmem_val;
	const gpio_pinmux_cfg_t *pinmux = NULL;

	if (!gpio_desc || !gpio_desc->pinmux)
		return -1;

	pinmux = gpio_desc->pinmux;
	devmem_fd = devmem_open((void *)pinmux->addr);
	if (devmem_fd <= 0)
		return -1;
	ret = devmem_read(devmem_fd, 32, &devmem_val);
	if (ret < 0)
		return -1;

	devmem_val &= ~(0x3 << pinmux->bit_offset);
	devmem_val |= (pinmux->val << pinmux->bit_offset);

	ret = devmem_write(devmem_fd, 32, devmem_val);
	if (ret < 0)
		return -1;
	ret = devmem_close(devmem_fd);
	if (ret < 0)
		return -1;

	return 0;
}

static int _gpio_set_dir_reg(const gpio_desc_t *gpio_desc)
{
	int devmem_fd, ret;
	uint32_t devmem_val;

	if (!gpio_desc || !gpio_desc->addr)
		return -1;

	devmem_fd = devmem_open((void *)gpio_desc->addr->dir_addr);
	if (devmem_fd <= 0)
		return -1;
	ret = devmem_read(devmem_fd, 32, &devmem_val);
	if (ret < 0)
		return -1;

	devmem_val &= ~(0x1 << gpio_desc->gpio.pin);
	if (gpio_desc->dir == GPIO_DIR_OUT)
		devmem_val |= (0x1 << gpio_desc->gpio.pin);

	ret = devmem_write(devmem_fd, 32, devmem_val);
	if (ret < 0)
		return -1;
	ret = devmem_close(devmem_fd);
	if (ret < 0)
		return -1;

	return 0;
}

static gpio_desc_t *_gpio_desc_init(const char *gpioname, uint8_t gpiodir)
{
	gpio_desc_t *gpio_desc = NULL;
	char strtmp[GPIO_NAME_LEN_MAX] = {0};
	char *str = NULL;
	int i;

	if (!gpioname)
		return NULL;

	gpio_desc = malloc(sizeof(gpio_desc_t));
	if (!gpio_desc) {
		perror("malloc gpio_desc");
		return NULL;
	}

	if (gpiodir != GPIO_DIR_IN && gpiodir != GPIO_DIR_OUT) {
		fprintf(stderr, "[ERROR] GPIO DIR error");
	}
	gpio_desc->dir = gpiodir;

	strncpy(strtmp, gpioname, GPIO_NAME_LEN_MAX);

	/* get gpio controller */
	str = strtok(strtmp, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO CTRL get error");
		goto err;
	}
	gpio_desc->gpio.ctrl = strtoul(str, NULL, 0);
	if (errno || gpio_desc->gpio.ctrl >= GPIO_CTRL_NUM) {
		fprintf(stderr, "[ERROR] GPIO CTRL format or range error");
		goto err;
	}

	/* get gpio port */
	str = strtok(str, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO PORT get error");
		goto err;
	}
	gpio_desc->gpio.port = tolower(str[0]);
	if (gpio_desc->gpio.port < 'a' || gpio_desc->gpio.port > 'b') {
		fprintf(stderr, "[ERROR] GPIO PORT range error");
		goto err;
	}

	/* get gpio pin */
	str = strtok(str, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO PIN get error");
		goto err;
	}
	gpio_desc->gpio.pin  = strtoul(str, NULL, 0);
	if (errno || gpio_desc->gpio.pin >= GPIO_PIN_NUM) {
		fprintf(stderr, "[ERROR] GPIO PIN format or range error");
		goto err;
	}

	/* get pinmux cfg */
	for (i = 0; i < ARRAY_SIZE(gpio_pinmux_cfg); i++) {
		if (gpio_pinmux_cfg[i].gpio.ctrl == gpio_desc->gpio.ctrl
		 && gpio_pinmux_cfg[i].gpio.port == gpio_desc->gpio.port
		 && gpio_pinmux_cfg[i].gpio.pin  == gpio_desc->gpio.pin) {

			gpio_desc->pinmux = &gpio_pinmux_cfg[i];
			break;
		}
	}
	if (!gpio_desc->pinmux) {
		fprintf(stderr, "[ERROR] GPIO pinmux_cfg is NULL");
		goto err;
	}

	/* get gpio addr info */
	for (i = 0; i < ARRAY_SIZE(gpio_addr); i++) {
		if (gpio_addr[i].gpio.ctrl == gpio_desc->gpio.ctrl
		 && gpio_addr[i].gpio.port == gpio_desc->gpio.port) {

			gpio_desc->addr = &gpio_addr[i];
			break;
		}
	}
	if (!gpio_desc->addr) {
		fprintf(stderr, "[ERROR] GPIO gpio_addr is NULL");
		goto err;
	}

	return gpio_desc;

err:
	if (gpio_desc) {
		free(gpio_desc);
		gpio_desc = NULL;
	}
	return NULL;
}

gpio_desc_t *gpio_open(const char *gpioname, uint8_t gpiodir)
{
	gpio_desc_t *gpio_desc = NULL;
	int ret;

	gpio_desc = _gpio_desc_init(gpioname, gpiodir);
	if (!gpio_desc) {
		fprintf(stderr, "[ERROR] GPIO Read desc failed.");
		return NULL;
	}

	ret = _gpio_set_pinmux_reg(gpio_desc);
	if (ret < 0) {
		fprintf(stderr, "[ERROR] GPIO Set pinmux reg failed.");
		return NULL;
	}

	ret = _gpio_set_dir_reg(gpio_desc);
	if (ret < 0) {
		fprintf(stderr, "[ERROR] GPIO Set dir reg failed.");
		return NULL;
	}

	return gpio_desc;
}

int gpio_close(gpio_desc_t *gpio_desc)
{
	if (!gpio_desc)
		return -1;

	free(gpio_desc);
	return 0;
}

int gpio_set(gpio_desc_t *gpio_desc, uint8_t val)
{
	int devmem_fd, ret;
	uint32_t devmem_val;

	if (!gpio_desc || !gpio_desc->addr)
		return -1;

	if (gpio_desc->dir != GPIO_DIR_OUT) {
		fprintf(stderr, "[ERROR] It's not a OUTPUT PIN.");
		return -1;
	}

	devmem_fd = devmem_open((void *)gpio_desc->addr->out_addr);
	if (devmem_fd <= 0)
		return -1;
	ret = devmem_read(devmem_fd, 32, &devmem_val);
	if (ret < 0)
		return -1;

	devmem_val &= ~(0x1 << gpio_desc->gpio.pin);
	if (val)
		devmem_val |= (0x1 << gpio_desc->gpio.pin);

	ret = devmem_write(devmem_fd, 32, devmem_val);
	if (ret < 0)
		return -1;
	ret = devmem_close(devmem_fd);
	if (ret < 0)
		return -1;

	return 0;
}

int gpio_get(gpio_desc_t *gpio_desc, uint8_t *val)
{
	int devmem_fd, ret;
	uint32_t devmem_val;
	void *gpio_addr;

	if (!gpio_desc || !gpio_desc->addr)
		return -1;

	if (gpio_desc->dir == GPIO_DIR_IN)
		gpio_addr = (void *)gpio_desc->addr->in_addr;
	else
		gpio_addr = (void *)gpio_desc->addr->out_addr;
	devmem_fd = devmem_open(gpio_addr);
	if (devmem_fd <= 0)
		return -1;

	ret = devmem_read(devmem_fd, 32, &devmem_val);
	if (ret < 0)
		return -1;
	*val = (devmem_val >> gpio_desc->gpio.pin) & 0x1;

	ret = devmem_close(devmem_fd);
	if (ret < 0)
		return -1;

	return 0;
}
