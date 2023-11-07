#ifndef __DEVMEM_H__
#define __DEVMEM_H__

#include <stdint.h>

int devmem_open(void *addr);
int devmem_close(int fd);
int devmem_read(int fd, uint8_t offset, uint8_t width, uint32_t *val);
int devmem_write(int fd, uint8_t offset, uint8_t width, uint32_t val);
int devmem_clr_bit(int fd, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_set_bit(int fd, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_get_bit(int fd, uint8_t offset, uint8_t width, uint8_t bit);

int devmem_read2(void *addr, uint8_t offset, uint8_t width, uint32_t *val);
int devmem_write2(void *addr, uint8_t offset, uint8_t width, uint32_t val);
int devmem_clr_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_set_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_get_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit);

int devmem_read_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint32_t *val);
int devmem_write_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint32_t val);
int devmem_clr_bit_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_set_bit_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_get_bit_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint8_t bit);

#endif /* __DEVMEM_H__ */
