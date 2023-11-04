#include <stdio.h>
#include <ctype.h>
int main(int argc, char *argv[])
{
	int i;
	char arr[] = "HelloWorld";

	printf("Orig  =%s\n", arr);

	for (i = 0; i < sizeof(arr); i++)
	{
		arr[i] = tolower(arr[i]);
		//arr[i] = toupper(arr[i]);
	}

	printf("Lower =%s\n", arr);

	return 0;
}
