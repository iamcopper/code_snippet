#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char gpiodesc[] = "00-ab-71a+89";

	printf("gpiodesc=%s\n", gpiodesc);

	char *str = strtok(gpiodesc, "-+");
	while (str)
	{
		printf("str=%s\n", str);
		str = strtok(NULL, "-+");
	}

	printf("gpiodesc=%s\n", gpiodesc);

	return 0;
}
