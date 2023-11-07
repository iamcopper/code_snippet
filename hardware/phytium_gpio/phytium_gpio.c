#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "phytium_gpio.h"
#include "devmem.h"

/* Phytium 2004/D2000 */
#define PINMUX_ADDR   (0x28180000 + 0x0200)
#define GPIO_0_ADDR   0x28004000
#define GPIO_1_ADDR   0x28005000

#define REG_WIDTH     32

/* GPIO CMP Flags */
#define GPIO_CMP_ALL       (GPIO_CMP_CTRL | GPIO_CMP_PORT | GPIO_CMP_PIN)
#define GPIO_CMP_CTRL_PORT (GPIO_CMP_CTRL | GPIO_CMP_PORT)
#define GPIO_CMP_CTRL      (1 << 2)
#define GPIO_CMP_PORT      (1 << 1)
#define GPIO_CMP_PIN       (1 << 0)

#define ARRAY_SIZE(array) \
	sizeof(array)/(sizeof(array[0]))

static gpio_pinmux_t gpio_pinmux[] = {
	{{0, 'a', 0}, NULL, 32, PINMUX_ADDR, 0x00, 25, 0x1, 0},
	{{0, 'a', 1}, NULL, 32, PINMUX_ADDR, 0x00, 21, 0x1, 0},
	{{0, 'a', 2}, NULL, 32, PINMUX_ADDR, 0x00, 17, 0x1, 0},
	{{0, 'a', 3}, NULL, 32, PINMUX_ADDR, 0x00, 13, 0x1, 0},
	{{0, 'a', 4}, NULL, 32, PINMUX_ADDR, 0x00,  9, 0x1, 0},
	{{0, 'a', 5}, NULL, 32, PINMUX_ADDR, 0x00,  5, 0x1, 0},
	{{0, 'a', 6}, NULL, 32, PINMUX_ADDR, 0x00,  1, 0x1, 0},
	{{0, 'a', 7}, NULL, 32, PINMUX_ADDR, 0x04, 29, 0x1, 0},
	{{0, 'b', 0}, NULL, 32, PINMUX_ADDR, 0x28, 25, 0x2, 0},
	{{0, 'b', 1}, NULL, 32, PINMUX_ADDR, 0x28, 21, 0x2, 0},
	{{0, 'b', 2}, NULL, 32, PINMUX_ADDR, 0x28, 17, 0x2, 0},
	{{0, 'b', 3}, NULL, 32, PINMUX_ADDR, 0x2C, 21, 0x2, 0},
	{{0, 'b', 4}, NULL, 32, PINMUX_ADDR, 0x2C, 17, 0x2, 0},
	{{0, 'b', 5}, NULL, 32, PINMUX_ADDR, 0x10,  1, 0x2, 0},
	{{0, 'b', 6}, NULL, 32, PINMUX_ADDR, 0x14,  8, 0x2, 0},
	{{0, 'b', 7}, NULL, 32, PINMUX_ADDR, 0x14,  5, 0x2, 0},
	{{1, 'a', 0}, NULL, 32, PINMUX_ADDR, 0x28, 13, 0x2, 0},
	{{1, 'a', 1}, NULL, 32, PINMUX_ADDR, 0x28,  8, 0x2, 0},
	{{1, 'a', 2}, NULL, 32, PINMUX_ADDR, 0x28,  5, 0x2, 0},
	{{1, 'a', 3}, NULL, 32, PINMUX_ADDR, 0x18, 13, 0x1, 0},
	{{1, 'a', 4}, NULL, 32, PINMUX_ADDR, 0x18,  9, 0x1, 0},
	{{1, 'a', 5}, NULL, 32, PINMUX_ADDR, 0x08, 17, 0x1, 0},
	{{1, 'a', 6}, NULL, 32, PINMUX_ADDR, 0x08, 13, 0x1, 0},
	{{1, 'a', 7}, NULL, 32, PINMUX_ADDR, 0x08,  9, 0x1, 0},
	{{1, 'b', 0}, NULL, 32, PINMUX_ADDR, 0x08,  5, 0x1, 0},
	{{1, 'b', 1}, NULL, 32, PINMUX_ADDR, 0x08,  1, 0x1, 0},
	{{1, 'b', 2}, NULL, 32, PINMUX_ADDR, 0x0C, 29, 0x1, 0},
	{{1, 'b', 3}, NULL, 32, PINMUX_ADDR, 0x0C, 25, 0x1, 0},
	{{1, 'b', 4}, NULL, 32, PINMUX_ADDR, 0x0C, 21, 0x1, 0},
	{{1, 'b', 5}, NULL, 32, PINMUX_ADDR, 0x0C, 17, 0x1, 0},
	{{1, 'b', 6}, NULL, 32, PINMUX_ADDR, 0x0C, 13, 0x1, 0},
	{{1, 'b', 7}, NULL, 32, PINMUX_ADDR, 0x00, 21, 0x1, 0},
};

static gpio_ctrl_t gpio_ctrl[] = {
	{{0, 'a', 0}, NULL, 32, GPIO_0_ADDR, 0x04, 0x08, 0x00},
	{{0, 'b', 0}, NULL, 32, GPIO_0_ADDR, 0x10, 0x14, 0x0C},
	{{1, 'a', 0}, NULL, 32, GPIO_1_ADDR, 0x04, 0x08, 0x00},
	{{1, 'b', 0}, NULL, 32, GPIO_1_ADDR, 0x10, 0x14, 0x0C},
};

static int _gpio_cmp(const gpio_t *gpio1, const gpio_t *gpio2, uint8_t flag)
{
	if ((flag & GPIO_CMP_CTRL) && (gpio1->ctrl != gpio2->ctrl)) {
		return (gpio1->ctrl - gpio2->ctrl);
	}

	if ((flag & GPIO_CMP_PORT) && (gpio1->port != gpio2->port))  {
		return (gpio1->port - gpio2->port);
	}

	if (flag & GPIO_CMP_PIN)  {
		return (gpio1->pin- gpio2->pin);
	}

	return 0;
}

static int _gpio_set_pinmux_reg(gpio_desc_t *gpio_desc)
{
	int ret;
	uint32_t devmem_val;
	gpio_pinmux_t *pinmux;

	if (!gpio_desc || !gpio_desc->pinmux)
		return -1;

	pinmux = gpio_desc->pinmux;
	ret = devmem_read(pinmux->devmem, pinmux->offset, pinmux->width, &devmem_val);
	if (ret < 0)
		return -1;

	/* phytium gpio pinmux: 2 bits */
	devmem_val &= ~(0x3 << pinmux->h_bit);
	devmem_val |= (pinmux->set_val << pinmux->h_bit);
	
	ret = devmem_write(pinmux->devmem, pinmux->offset, pinmux->width, devmem_val);
	if (ret < 0)
		return -1;

	pinmux->val = pinmux->set_val;
	return 0;
}

static int _gpio_get_pinmux_reg(gpio_desc_t *gpio_desc, uint8_t *val)
{
	int ret;
	uint32_t devmem_val;
	gpio_pinmux_t *pinmux;

	if (!gpio_desc || !gpio_desc->pinmux)
		return -1;

	pinmux = gpio_desc->pinmux;
	ret = devmem_read(pinmux->devmem, pinmux->offset, pinmux->width, &devmem_val);
	if (ret < 0)
		return -1;

	pinmux->val = (devmem_val >> (pinmux->h_bit - 1)) & 0x3;
	if (val)
		*val = pinmux->val;
	return 0;
}

static int _gpio_set_dir_reg(gpio_desc_t *gpio_desc, uint8_t dir)
{
	int ret;
	uint32_t devmem_val;
	gpio_ctrl_t *ctrl;

	if (!gpio_desc || !gpio_desc->ctrl || (dir != GPIO_DIR_IN && dir != GPIO_DIR_OUT))
		return -1;

	ctrl = gpio_desc->ctrl;

	ret = devmem_read(ctrl->devmem, ctrl->dir_offset, ctrl->width, &devmem_val);
	if (ret < 0)
		return -1;

	devmem_val &= ~(0x1 << gpio_desc->gpio.pin);
	if (dir == GPIO_DIR_OUT)
		devmem_val |= (0x1 << gpio_desc->gpio.pin);

	ret = devmem_write(ctrl->devmem, ctrl->dir_offset, ctrl->width, devmem_val);
	if (ret < 0)
		return -1;

	gpio_desc->dir = dir;
	return 0;
}

static int _gpio_get_dir_reg(gpio_desc_t *gpio_desc, uint8_t *dir)
{
	int ret;
	uint32_t devmem_val;
	gpio_ctrl_t *ctrl;

	if (!gpio_desc || !gpio_desc->ctrl)
		return -1;

	ctrl = gpio_desc->ctrl;
	ret = devmem_read(ctrl->devmem, ctrl->dir_offset, 32, &devmem_val);
	if (ret < 0)
		return -1;

	if ((devmem_val >> gpio_desc->gpio.pin) & 0x1)
		gpio_desc->dir = GPIO_DIR_OUT;
	else
		gpio_desc->dir = GPIO_DIR_IN;

	if (dir)
		*dir = gpio_desc->dir;
	return 0;
}

static int _gpio_set_val_reg(gpio_desc_t *gpio_desc, uint8_t val)
{
	int ret;
	uint32_t devmem_val;
	gpio_ctrl_t *ctrl;

	if (!gpio_desc || !gpio_desc->ctrl)
		return -1;

	ctrl = gpio_desc->ctrl;
	ret = devmem_read(ctrl->devmem, ctrl->out_offset, 32, &devmem_val);
	if (ret < 0)
		return -1;

	devmem_val &= ~(0x1 << gpio_desc->gpio.pin);
	if (val)
		devmem_val |= (0x1 << gpio_desc->gpio.pin);

	ret = devmem_write(ctrl->devmem, ctrl->out_offset, 32, devmem_val);
	if (ret < 0)
		return -1;

	gpio_desc->val = val;
	return 0;
}

static int _gpio_get_val_reg(gpio_desc_t *gpio_desc, uint8_t *val)
{
	int ret;
	uint32_t devmem_val;
	uint8_t offset, dir;

	if (!gpio_desc || !gpio_desc->ctrl)
		return -1;

	ret = _gpio_get_dir_reg(gpio_desc, &dir);
	if (ret)
		return -1;

	if (dir == GPIO_DIR_IN)
		offset = gpio_desc->ctrl->in_offset;
	else
		offset = gpio_desc->ctrl->out_offset;

	ret = devmem_read(gpio_desc->ctrl->devmem, offset, gpio_desc->ctrl->width, &devmem_val);
	if (ret < 0)
		return -1;

	gpio_desc->val = (devmem_val >> gpio_desc->gpio.pin) & 0x1;
	if (val)
		*val = gpio_desc->val;

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
		fprintf(stderr, "[ERROR] GPIO CTRL get error");
		return -1;
	}
	gpio_desc->gpio.ctrl = strtoul(str, NULL, 0);
	if (errno || gpio_desc->gpio.ctrl >= GPIO_CTRL_NUM) {
		fprintf(stderr, "[ERROR] GPIO CTRL format or range error");
		return -1;
	}

	/* get gpio port */
	str = strtok(str, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO PORT get error");
		return -1;
	}
	gpio_desc->gpio.port = tolower(str[0]);
	if (gpio_desc->gpio.port < 'a' || gpio_desc->gpio.port > 'b') {
		fprintf(stderr, "[ERROR] GPIO PORT range error");
		return -1;
	}

	/* get gpio pin */
	str = strtok(str, "-");
	if (!str) {
		fprintf(stderr, "[ERROR] GPIO PIN get error");
		return -1;
	}
	gpio_desc->gpio.pin  = strtoul(str, NULL, 0);
	if (errno || gpio_desc->gpio.pin >= GPIO_PIN_NUM) {
		fprintf(stderr, "[ERROR] GPIO PIN format or range error");
		return -1;
	}

	/* get pinmux cfg */
	for (i = 0; i < ARRAY_SIZE(gpio_pinmux); i++) {
		if (0 == _gpio_cmp(&gpio_desc->gpio, &gpio_pinmux[i].gpio, GPIO_CMP_ALL)) {
			gpio_desc->pinmux = &gpio_pinmux[i];
			break;
		}
	}
	if (!gpio_desc->pinmux) {
		fprintf(stderr, "[ERROR] GPIO pinmux_cfg is NULL");
		return -1;
	}

	/* get gpio addr info */
	for (i = 0; i < ARRAY_SIZE(gpio_ctrl); i++) {
		if (0 == _gpio_cmp(&gpio_desc->gpio, &gpio_ctrl[i].gpio, GPIO_CMP_CTRL_PORT)) {
			gpio_desc->ctrl = &gpio_ctrl[i];
			break;
		}
	}
	if (!gpio_desc->ctrl) {
		fprintf(stderr, "[ERROR] GPIO gpio_addr is NULL");
		return -1;
	}

	return 0;
}

static int _check_pinmux(gpio_desc_t *gpio_desc)
{
	uint8_t pinmux_val;
	int ret;

	if (!gpio_desc)
		return -1;

	ret = _gpio_get_pinmux_reg(gpio_desc, &pinmux_val);
	if (ret < 0)
		return -1;

	if (pinmux_val != gpio_desc->pinmux->set_val) {
		fprintf(stderr, "[WARN] GPIO pinmux not read, now set it.\n");
		return _gpio_set_pinmux_reg(gpio_desc);
	}

	return 0;
}

gpio_desc_t *gpio_open(const char *gpioname)
{
	gpio_desc_t *gpio_desc;
	int ret;

	gpio_desc = malloc(sizeof(gpio_desc_t));
	if (!gpio_desc) {
		perror("malloc gpio_desc");
		return NULL;
	}

	ret = _gpio_desc_init(gpio_desc, gpioname);
	if (ret) {
		fprintf(stderr, "[ERROR] GPIO desc init failed.");
		goto err1;
	}

	gpio_desc->pinmux->devmem = devmem_open((void *)gpio_desc->pinmux->baseaddr);
	if (!gpio_desc->pinmux->devmem) {
		fprintf(stderr, "[ERROR] devmem_open() for pinmux error");
		goto err1;
	}

	gpio_desc->ctrl->devmem = devmem_open((void *)gpio_desc->ctrl->baseaddr);
	if (!gpio_desc->ctrl->devmem) {
		fprintf(stderr, "[ERROR] devmem_open() for pinmux error");
		goto err2;
	}

	/* get gpio pinmux from Hardware */
	if (0 > _gpio_get_pinmux_reg(gpio_desc, NULL))
		goto err3;

	/* get gpio dir from Hardware */
	if (0 > _gpio_get_dir_reg(gpio_desc, NULL))
		goto err3;

	/* get gpio value from Hardware */
	if (0 > _gpio_get_val_reg(gpio_desc, NULL))
		goto err3;

	return gpio_desc;

err3:
	devmem_close(gpio_desc->ctrl->devmem);
err2:
	devmem_close(gpio_desc->pinmux->devmem);
err1:
	free(gpio_desc);
	return NULL;
}

int gpio_close(gpio_desc_t *gpio_desc)
{
	devmem_close(gpio_desc->pinmux->devmem);

	devmem_close(gpio_desc->ctrl->devmem);

	free(gpio_desc);
	return 0;
}

int gpio_set_dir(gpio_desc_t *gpio_desc, uint8_t dir)
{
	if (0 > _check_pinmux(gpio_desc))
		return -1;

	return _gpio_set_dir_reg(gpio_desc, dir);
}

int gpio_set_val(gpio_desc_t *gpio_desc, uint8_t val)
{
	if (0 > _check_pinmux(gpio_desc))
		return -1;

	return _gpio_set_val_reg(gpio_desc, val);
}

int gpio_get_dir(gpio_desc_t *gpio_desc, uint8_t *dir)
{
	if (0 > _check_pinmux(gpio_desc))
		return -1;

	return _gpio_get_dir_reg(gpio_desc, dir);
}

int gpio_get_val(gpio_desc_t *gpio_desc, uint8_t *val)
{
	if (0 > _check_pinmux(gpio_desc))
		return -1;

	return _gpio_get_val_reg(gpio_desc, val);
}

int gpio_get_dir_val(gpio_desc_t *gpio_desc, uint8_t *dir, uint8_t *val)
{
	int ret;

	if (0 > _check_pinmux(gpio_desc))
		return -1;

	ret = _gpio_get_dir_reg(gpio_desc, dir);
	if (ret < 0)
		return ret;
	
	return _gpio_get_val_reg(gpio_desc, val);
}

int gpio_set_dir2(const char *gpioname, uint8_t dir)
{
	int ret;
	gpio_desc_t *gpio_desc;
	
	gpio_desc = gpio_open(gpioname);
	if (!gpio_desc)
		return -1;

	ret = gpio_set_dir(gpio_desc, dir);
	if (ret < 0)
		goto err;

	return gpio_close(gpio_desc);

err:
	gpio_close(gpio_desc);
	return -1;
}

int gpio_set_val2(const char *gpioname, uint8_t val)
{
	int ret;
	gpio_desc_t *gpio_desc;
	
	gpio_desc = gpio_open(gpioname);
	if (!gpio_desc)
		return -1;

	ret = gpio_set_val(gpio_desc, val);
	if (ret < 0)
		goto err;

	return gpio_close(gpio_desc);

err:
	gpio_close(gpio_desc);
	return -1;
}

int gpio_get_dir2(const char *gpioname, uint8_t *dir)
{
	int ret;
	gpio_desc_t *gpio_desc;
	
	gpio_desc = gpio_open(gpioname);
	if (!gpio_desc)
		return -1;

	ret = gpio_get_dir(gpio_desc, dir);
	if (ret < 0)
		goto err;

	return gpio_close(gpio_desc);

err:
	gpio_close(gpio_desc);
	return -1;
}

int gpio_get_val2(const char *gpioname, uint8_t *val)
{
	int ret;
	gpio_desc_t *gpio_desc;
	
	gpio_desc = gpio_open(gpioname);
	if (!gpio_desc)
		return -1;

	ret = gpio_get_val(gpio_desc, val);
	if (ret < 0)
		goto err;

	return gpio_close(gpio_desc);

err:
	gpio_close(gpio_desc);
	return -1;
}

int gpio_get_dir_val2(const char *gpioname, uint8_t *dir, uint8_t *val)
{
	int ret;
	gpio_desc_t *gpio_desc;
	
	gpio_desc = gpio_open(gpioname);
	if (!gpio_desc)
		return -1;

	ret = gpio_get_dir_val(gpio_desc, dir, val);
	if (ret < 0)
		goto err;

	return gpio_close(gpio_desc);

err:
	gpio_close(gpio_desc);
	return -1;
}
