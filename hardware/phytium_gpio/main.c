#include <stdio.h>

void usage(char *appname)
{
	printf("\nPhytium 2004/D2000 GPIO Control\n\n");
	printf("Usage: %s <gpio> <cmd> [args]\n\n", appname);
	printf("Options:\n");
	printf("<gpio> can be:\n");
	printf("  controller-port-pin, eg: 1-b-7\n");
	printf("<cmd> can be:\n");
	printf("  init <dir> ---  init a gpio pin with the direction in/out\n");
	printf("  set  <val> ---  set a gpio pin value to 0/1\n");
	printf("  get        ---  get a gpio pin value\n\n");
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
