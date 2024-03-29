#ifndef __I2CDEV_H__
#define __I2CDEV_H__

#include <stdint.h>

int open_i2cdev(int i2cbus, char *filename, size_t size, int quiet);
int close_i2cdev(int fd);

int i2cread(int fd, uint8_t devaddr, uint16_t regaddr, uint8_t *regval);
int i2cwrite(int fd, uint8_t devaddr, uint16_t regaddr, uint8_t regval);

#endif /* __I2CDEV_H__ */
