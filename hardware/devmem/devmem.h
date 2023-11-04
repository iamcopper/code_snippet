#ifndef __DEVMEM_H__
#define __DEVMEM_H__

#include <stdint.h>

int devmem_open(void *addr);
int devmem_close(int fd);
int devmem_read(int fd, uint8_t width, uint32_t *val);
int devmem_write(int fd, uint8_t width, uint32_t val);

int devmem_read2(void *addr, uint8_t width, uint32_t *val);
int devmem_write2(void *addr, uint8_t width, uint32_t val);

#endif /* __DEVMEM_H__ */
