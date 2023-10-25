#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include "i2cbusses.h"

int check_funcs(int fd)
{
	unsigned long funcs;

	/* check adapter functionality */
	if (ioctl(fd, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr, "Error: Could not get the adapter "
			"functionality matrix: %s\n", strerror(errno));
		return -1;
	}

	if (!(funcs & I2C_FUNC_I2C)) {
		fprintf(stderr, MISSING_FUNC_FMT, "I2C transfers");
		return -1;
	}

	return 0;
}

int i2cread(int fd, uint8_t devaddr, uint16_t regaddr, uint8_t *regval)
{
	struct i2c_msg msgs[2] = {0};
	uint8_t msg_buf_w[2] = {(regaddr >> 8) & 0xFF, regaddr & 0xFF};
	uint8_t msg_buf_r[1] = {0};
	struct i2c_rdwr_ioctl_data rdwr;
	int nmsgs = 0, nmsgs_sent = 0, flags = 0;

	msgs[nmsgs].addr = devaddr;
	msgs[nmsgs].flags = flags;           /* write */
	msgs[nmsgs].len = 2;                 /* 2 bytes */
	msgs[nmsgs].buf = msg_buf_w;
	nmsgs++;

	msgs[nmsgs].addr = devaddr;
	msgs[nmsgs].flags = flags | I2C_M_RD;/* read */
	msgs[nmsgs].len = 1;                 /* 1 byte */
	msgs[nmsgs].buf = msg_buf_r;
	nmsgs++;

	rdwr.msgs = msgs;
	rdwr.nmsgs = nmsgs;

	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent < 0 || nmsgs_sent < nmsgs) {
		fprintf(stderr, "Error: Sending messages failed: %s\n", strerror(errno));
		return -1;
	}

	*regval = msg_buf_r[0];

	return 0;
}

int i2cwrite(int fd, uint8_t devaddr, uint16_t regaddr, uint8_t val)
{
	struct i2c_msg msgs[1] = {0};
	uint8_t msg_buf_w[3] = {(regaddr >> 8) & 0xFF, regaddr & 0xFF, val};
	struct i2c_rdwr_ioctl_data rdwr;
	int nmsgs = 0, nmsgs_sent = 0, flags = 0;

	msgs[nmsgs].addr = devaddr;
	msgs[nmsgs].flags = flags;           /* write */
	msgs[nmsgs].len = 3;                 /* 3 bytes */
	msgs[nmsgs].buf = msg_buf_w;
	nmsgs++;

	rdwr.msgs = msgs;
	rdwr.nmsgs = nmsgs;

	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent < 0 || nmsgs_sent < nmsgs) {
		fprintf(stderr, "Error: Sending messages failed: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	char filename[20] = {0};
	int fd, i2cbus = 30;
	int ret = 0;
	uint16_t regaddr;
	uint8_t devaddr, regval;

	fd = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);
	if (fd < 0 || check_funcs(fd))
		exit(1);

	//devaddr = strtol(argv[2], NULL, 16);
	//regaddr = strtol(argv[3], NULL, 16);
	devaddr = 0x4a;
	regaddr = 0x0001;

	ret = i2cread(fd, devaddr, regaddr, &regval);
	printf("[read1 ret=%d] regaddr=0x%04x, regval=0x%02x\n", ret, regaddr, regval);

	regval = 0x01;
	ret = i2cwrite(fd, devaddr, regaddr, regval);
	printf("[write ret=%d] regaddr=0x%04x, regval=0x%02x\n", ret, regaddr, regval);

	regval = 0;
	ret = i2cread(fd, devaddr, regaddr, &regval);
	printf("[read2 ret=%d] regaddr=0x%04x, regval=0x%02x\n", ret, regaddr, regval);

	close(fd);

	exit(0);
}
