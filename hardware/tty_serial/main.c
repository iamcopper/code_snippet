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

#define BUFSIZE 256

void usage(char *appname)
{
	printf("Usage: %s </dev/ttyXXX>\n\n", appname);
}

void show_data(char buf[], int len)
{
	int i = 0;
	for(i = 0; i < len; i++) {
		printf(" 0x%02x", buf[i]);
		if ((i & 0xF) == 0xF)
			printf("\n");
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	int fd = 0;
	int ret = 0, i = 0;
	char buf[BUFSIZE] = {0};

	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
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
	printf(">>> %d bytes send.\n", ret);
	show_data(buf, ret);

	/* Receive Raw Data */
	memset(buf, 0, sizeof(buf));
	ret = read(fd, buf, sizeof(buf));
	if (ret <  0) {
		perror("read");
		goto err;
	}
	printf("<<< %d bytes received.\n", ret);
	show_data(buf, ret);

err:
	close(fd);

	return 0;
}
