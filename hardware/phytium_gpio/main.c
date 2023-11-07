#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "phytium_gpio.h"

#define CMD_INIT "init"
#define CMD_SET  "set"
#define CMD_GET  "get"

#define CMD_INIT_DIR_IN  "in"
#define CMD_INIT_DIR_OUT "out"

void usage(char *appname)
{
	printf("\nPhytium 2004/D2000 GPIO Control\n\n");
	printf("Usage: %s <cmd> <gpiodesc> [args]\n\n", appname);
	printf("Options:\n");
	printf("<gpiodesc> can be:\n");
	printf("  ctrl-port-pin, eg: 1-b-7\n");
	printf("  ctrl : 0-1\n");
	printf("  port : a, A, b, B\n");
	printf("  pin  : 0-7\n");
	printf("<cmd> and [args] are corresponding :\n");
	printf("  init in/out ---  init gpio pin as in/out\n");
	printf("  set  0/1    ---  set gpio pin to 0/1\n");
	printf("  get         ---  get gpio pin value\n\n");
	printf("Examples:\n");
	printf("%s init 1-b-7 out\n", appname);
	printf("%s set  1-b-7 1\n", appname);
	printf("%s get  1-b-7\n", appname);
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		usage(argv[0]);
		return 1;
	}

	const char *cmd = argv[1];
	const char *gpioname = argv[2];

	if (0 == strcmp(cmd, CMD_INIT)) {
		if (argc < 4) {
			printf("[ERROR] Missing args (in/out) for \"init\" cmd.\n");
			usage(argv[0]);
			return 1;
		}

		const char *dirstr = argv[3];
		uint8_t dir = 0;
		int ret;

		if (0 == strcmp(dirstr, CMD_INIT_DIR_IN)) {
			dir = GPIO_DIR_IN;
		} else if (0 == strcmp(dirstr, CMD_INIT_DIR_OUT)) {
			dir = GPIO_DIR_OUT;
		} else {
			printf("[ERROR] Args error for \"init\" cmd: %s\n", dirstr);
			usage(argv[0]);
			return 1;
		}

		ret = gpio_init(gpioname, dir);
		if (ret) {
			printf("[ERROR] gpio_get() failed.\n");
			return 1;
		}

		printf("[GPIO Init] %s : %s\n", gpioname, dirstr);
		return 0;
	} else if (0 == strcmp(cmd, CMD_SET)) {
		if (argc < 4) {
			printf("[ERROR] Missing args (0/1) for \"set\" cmd.\n");
			usage(argv[0]);
			return 1;
		}

		uint8_t val = strtoul(argv[3], NULL, 0);
		if (errno || val > 1) {
			printf("[ERROR] Args error for \"set\" cmd: %d\n", val);
			usage(argv[0]);
			return 1;
		}

		if (gpio_set(gpioname, val)) {
			printf("[ERROR] gpio_get() failed.\n");
			return 1;
		}

		printf("[GPIO Set] %s : 0x%x\n", gpioname, val);
		return 0;
	} else if (0 == strcmp(cmd, CMD_GET)) {
		uint8_t val;
		if (gpio_get(gpioname, &val)) {
			printf("[ERROR] gpio_get() failed.\n");
			return 1;
		}

		printf("[GPIO Get] %s : 0x%x\n", gpioname, val);
		return 0;
	} else {
		printf("[ERROR] Unsupport cmd: %s.\n", cmd);
		usage(argv[0]);
		return 1;
	}

	return 0;
}
