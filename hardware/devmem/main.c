#include <stdio.h>

void usage(char *appname)
{
	printf("\nRead/write from physical address\n\n");
	printf("Usage: %s <address> [<width> [value]]\n\n", appname);
	printf("  address --- address to read or write\n");
    printf("  width   --- width (8/16/32)\n");
    printf("  value   --- value to be written\n\n");
}

int main(int argc, char *argv[])
{
	usage(argv[0]);
	return 0;
}
