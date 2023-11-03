#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "devmem.h"

void usage(char *appname)
{
	printf("\nRead/write from physical address\n\n");
	printf("Usage: %s <address> [<width> [value]]\n\n", appname);
	printf("  address --- address to read or write\n");
    printf("  width   --- width 8/16/[32]\n");
    printf("  value   --- value to be written\n\n");
}

int main(int argc, char *argv[])
{
	void *addr;
	uint8_t width = 32;
	uint32_t val;
	int fd, ret;

	if (argc < 2 || argc > 4) {
		usage(argv[0]);
		return 1;
	}

	addr = (void *)strtoul(argv[1], NULL, 0);
	if (errno) {
		perror("Invalid address specified\n");
		return 1;
	}

	if (argc > 2) {
		width = strtoul(argv[2], NULL, 0);
		if (errno && width != 8 && width != 16 && width != 32) {
			fprintf(stderr, "Invalid width specified\n");
			return 1;
		}
	}

	fd = devmem_open(addr);
	if (fd <= 0) {
		fprintf(stderr, "[ERROR] devmem_open failed\n");
		return 1;
	}

	if (argc < 4) {
		/* DEVMEM Read */
		ret = devmem_read(fd, width, &val);
		if (ret < 0)
			fprintf(stderr, "[ERROR] devmem_read failed\n");

		switch (width) {
			case 8:
				printf("0x%01x\n", val);
				break;
			case 16:
				printf("0x%02x\n", val);
				break;
			case 32:
				printf("0x%04x\n", val);
				break;
		}
	} else {
		/* DEVMEM Write */
		val = strtoul(argv[3], NULL, 0);
		if (errno) {
			perror("Invalid val specified\n");
			return 1;
		}

		ret = devmem_write(fd, width, val);
		if (ret < 0)
			fprintf(stderr, "[ERROR] devmem_read failed\n");
	}

	ret = devmem_close(fd);
	if (ret < 0)
		fprintf(stderr, "[ERROR] devmem_close failed\n");

	return ret;
}
