#define __TEST__

#include <stdio.h>
#include <stdint.h>
#include "fpga_socket.h"

#ifdef __TEST__

#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
	int fd;

	fd = fpga_open("eth0");

	ssize_t ret;
	uint8_t buf[BUF_SIZE] = {0};

	while (1) {
		printf("Waitint for read:\n");
		ret = fpga_read(fd, buf, BUF_SIZE, 0);
		printf("fpga_read() ret=%ld\n", ret);
		if (ret > 0) {
			ret = fpga_write(fd, buf, ret, 0);
			printf("fpga_read() ret=%ld\n", ret);
		}
	}

	fpga_close(fd);

	return 0;
}

#endif /* __TEST__ */
