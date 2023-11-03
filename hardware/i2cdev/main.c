#include <stdio.h>
#include <stdlib.h>
#include "i2cdev.h"

/**
 *  Based on i2c-tools/tools/i2ctransfer.c
 */

int main(int argc, char *argv[])
{
	char filename[20] = {0};
	int fd, i2cbus;
	int ret = 0;
	uint16_t regaddr;
	uint8_t devaddr, regval;

	i2cbus = 30;
	devaddr = 0x4a;
	regaddr = 0x0010;
	printf("i2cbus  : %d (/dev/i2c-%d)\n", i2cbus, i2cbus);
	printf("devaddr : 0x%02x\n", devaddr);
	printf("regaddr : 0x%04x\n", regaddr);

	fd = open_i2cdev(i2cbus, filename, sizeof(filename), 0);
	if (fd < 0)
		exit(1);

	ret = i2cread(fd, devaddr, regaddr, &regval);
	printf("[read1 ret=%d] regaddr=0x%04x, regval=0x%02x\n", ret, regaddr, regval);

	if (regval == 0x01)
		regval = 0x02;
	else
		regval = 0x01;
	ret = i2cwrite(fd, devaddr, regaddr, regval);
	printf("[write ret=%d] regaddr=0x%04x, regval=0x%02x\n", ret, regaddr, regval);

	regval = 0;
	ret = i2cread(fd, devaddr, regaddr, &regval);
	printf("[read2 ret=%d] regaddr=0x%04x, regval=0x%02x\n", ret, regaddr, regval);

	close_i2cdev(fd);

	exit(0);
}
