#include <stdio.h>

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
	usage(argv[0]);
	return 0;
}
