#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int ret = 0;

	const char *str1 = "stdout stream: aaaaaaa\n";
	const char *str2 = "stderr stream: bbbbbbb\n";
	ret = write(STDOUT_FILENO, str1, strlen(str1));
	if (ret < 0)
		perror("write1");
	else
		printf("ret1=%d\n", ret);

	ret = write(STDERR_FILENO, str2, strlen(str2));
	if (ret < 0)
		perror("write2");
	else
		printf("ret2=%d\n", ret);

	return 0;
}
