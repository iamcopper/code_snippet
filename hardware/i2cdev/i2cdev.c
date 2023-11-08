#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include "i2cdev.h"

/* Copied from i2c-tools/tools/i2ctransfer.c */
#define MISSING_FUNC_FMT	"Error: Adapter does not have %s capability\n"
static int check_funcs(int fd)
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

/* Copied from i2c-tools/tools/i2cbusses.c */
int open_i2cdev(int i2cbus, char *filename, size_t size, int quiet)
{
	int file, len;

	len = snprintf(filename, size, "/dev/i2c/%d", i2cbus);
	if (len >= (int)size) {
		fprintf(stderr, "%s: path truncated\n", filename);
		return -EOVERFLOW;
	}
	file = open(filename, O_RDWR);

	if (file < 0 && (errno == ENOENT || errno == ENOTDIR)) {
		len = snprintf(filename, size, "/dev/i2c-%d", i2cbus);
		if (len >= (int)size) {
			fprintf(stderr, "%s: path truncated\n", filename);
			return -EOVERFLOW;
		}
		file = open(filename, O_RDWR);
	}

	if (file < 0 && !quiet) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"`/dev/i2c-%d' or `/dev/i2c/%d': %s\n",
				i2cbus, i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
	}

	if (file > 0 && check_funcs(file)) {
		close(file);
		return -EIO;
	}

	return file;
}

int close_i2cdev(int fd)
{
	return close(fd);
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

int i2cwrite(int fd, uint8_t devaddr, uint16_t regaddr, uint8_t regval)
{
	struct i2c_msg msgs[1] = {0};
	uint8_t msg_buf_w[3] = {(regaddr >> 8) & 0xFF, regaddr & 0xFF, regval};
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

