#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "phytium_gpio.h"

#define CMD_ARG_DIR      "dir"
#define CMD_ARG_VAL      "val"
#define CMD_ARG_DIR_IN   "in"
#define CMD_ARG_DIR_OUT  "out"

void usage(char *appname)
{
	printf("\nUsage: %s <opts> <gpio> [args]\n", appname);
	printf("Phytium 2004/D2000 GPIO Control\n\n");
	printf("opts:\n");
	printf("  -l [gpio]                  list GPIOs infomation\n");
	printf("  -s dir <gpioname> <in/out> set the GPIO direction: in, out\n");
	printf("  -s val <gpioname> <0/1>    set the GPIO value: 0, 1 (only out pin can set).\n");
	printf("  -g dir <gpioname>          get the GPIO data\n");
	printf("  -g dir <gpioname>          get the GPIO data\n");
	printf("  -h                         print this usage\n\n");
	printf("gpio name string format:\n");
	printf("  ctrl-port-pin, eg: 1-b-7\n");
	printf("  ctrl : 0, 1\n");
	printf("  port : a, b\n");
	printf("  pin  : 0 ~ 7\n");
}

int main(int argc, char *argv[])
{
	int argflag = 0;

	while ((argflag = getopt(argc, (char **)argv, "ls:g:h")) != -1) {
		switch (argflag) {
		case 'l':
			break;
		case 's':
			break;
		case 'g':
			break;
		case 'h':
		default:
			usage(argv[0]);
			return 0;
			break;
		}
	}

	printf("argc: %d\n", argc);
	for (int i = 0; i < argc; i++)
		printf("argv[%d]: %s\n", i, argv[i]);

	return 0;
}
