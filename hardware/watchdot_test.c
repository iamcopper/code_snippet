/**
 * Base on kernel source code tree:
 * watchdog-simple.c and do some personal changes.
 *
 * Kernel Test Demo:
 * <KERNEL_SRC>/samples/watchdog/watchdog-simple.c
 *
 * Kernel Document:
 * https://www.kernel.org/doc/html/latest/watchdog/wdt.html
 * https://www.kernel.org/doc/html/latest/watchdog/watchdog-api.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/watchdog.h>

#define WATCHDOG     "/dev/watchdog"
#define INTEVAL_SEC  1

void feed(int fd)
{
	ioctl(fd, WDIOC_KEEPALIVE, 0);
}

void set_timeout(int fd, int timeout)
{
	ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
	printf("The timeout was set to %d seconds\n", timeout);
}

int get_timeout(int fd)
{
	int timeout = 0;
	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	printf("timeout = %d\n", timeout);
	return timeout;
}

int get_timeleft(int fd)
{
	int timeleft = 0;
	ioctl(fd, WDIOC_GETTIMELEFT, &timeleft);
	printf("timeleft = %d\n", timeleft);
	return timeleft;
}

int main(void)
{
	int ret = 0;
	int count = 0;

	printf("Open watchdog: %s\n", WATCHDOG);
	int fd = open(WATCHDOG, O_WRONLY);
	if (fd == -1) {
		perror("watchdog open");
		exit(EXIT_FAILURE);
	}
	while (1) {
		printf("Feed watchdog: count=%d\n", count++);
		ret = write(fd, "\0", 1);
		if (ret != 1) {
			perror("watchdog write");
			ret = -1;
			break;
		}
		sleep(INTEVAL_SEC);
	}
	printf("Close watchdog.\n");
	close(fd);
	return ret;
}
