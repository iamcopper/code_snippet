#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include "tty_serial.h"

#define TTY_SERIAL_DEV   "/dev/ttyTHS3"
#define BUFSIZE          256

int main(int argc, char *argv[])
{
	int fd = 0;
	int ret = 0, i = 0;
	char buf[BUFSIZE] = {0};

	fd = open(TTY_SERIAL_DEV, O_RDWR);
	if(fd < 0) {
		perror("open");
		return -1;
	}

	/* Setting TTY Serial to Raw Mode */
	ret = tty_serial_init(fd);
	if (ret < 0) {
		fprintf(stderr, "TTY Serial init error.\n");
		goto err;
	}

	/* Send Raw Data */
	i = 0;
	buf[i++] = 0x01;
	buf[i++] = 0x02;
	buf[i++] = 0x03;
	buf[i++] = 0x04;
	ret = write(fd, buf, i);
	if (ret <  0) {
		perror("write");
		goto err;
	}

	/* Receive Raw Data */
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, sizeof(buf));
	if (ret <  0) {
		perror("read");
		goto err;
	}

err:
	close(fd);

	return 0;
}
