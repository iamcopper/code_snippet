#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "devmem.h"

void usage(char *appname)
{
	printf("\nUsage: %s <address> <offset> [<width> [value]]\n", appname);
	printf("Read/write from physical address\n\n");
	printf("  address --- address to read or write\n");
	printf("  offset  --- address offset, 0~255 [0]\n");
	printf("  width   --- width 8/16/[32]\n");
	printf("  value   --- value to be written\n\n");
}

int main(int argc, char *argv[])
{
	void *addr;
	uint8_t offset = 0x00, width = 32;
	uint32_t val;
	int ret;

	if (argc < 3 || argc > 5) {
		usage(argv[0]);
		return 1;
	}

	addr = (void *)strtoul(argv[1], NULL, 0);
	if (errno) {
		perror("Invalid address specified\n");
		return 1;
	}

	offset = strtoul(argv[2], NULL, 0);
	if (errno) {
		perror("Invalid address specified\n");
		return 1;
	}

	if (argc >= 4) {
		width = strtoul(argv[3], NULL, 0);
		if (errno && width != 8 && width != 16 && width != 32) {
			fprintf(stderr, "Invalid width specified\n");
			return 1;
		}
	}

	if (argc < 5) {
		/* DEVMEM Read */
		ret = devmem_read2(addr, offset, width, &val);
		if (ret < 0) {
			fprintf(stderr, "[ERROR] devmem_read2 failed(ret=%d)\n", ret);
			return 1;
		}

		switch (width) {
			case 8:
				printf("0x%02x\n", val);
				break;
			case 16:
				printf("0x%04x\n", val);
				break;
			case 32:
				printf("0x%08x\n", val);
				break;
		}
	} else {
		/* DEVMEM Write */
		val = strtoul(argv[4], NULL, 0);
		if (errno) {
			perror("Invalid val specified\n");
			return 1;
		}

		ret = devmem_write2(addr, offset, width, val);
		if (ret < 0) {
			fprintf(stderr, "[ERROR] devmem_write2 failed(ret=%d)\n", ret);
			return 1;
		}
	}

	return ret;
}
