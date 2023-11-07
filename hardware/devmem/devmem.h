#ifndef __DEVMEM_H__
#define __DEVMEM_H__

#include <stdint.h>

typedef struct devmem {
	void   *addr;
	void   *vt_addr;
	void   *vt_base_addr;
	size_t map_size;
	int    fd;
} devmem_t;

devmem_t *devmem_open(void *addr);
int devmem_close(devmem_t *devmem);
int devmem_read(devmem_t *devmem, uint8_t offset, uint8_t width, uint32_t *val);
int devmem_write(devmem_t *devmem, uint8_t offset, uint8_t width, uint32_t val);
int devmem_clr_bit(devmem_t *devmem, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_set_bit(devmem_t *devmem, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_get_bit(devmem_t *devmem, uint8_t offset, uint8_t width, uint8_t bit);

int devmem_read2(void *addr, uint8_t offset, uint8_t width, uint32_t *val);
int devmem_write2(void *addr, uint8_t offset, uint8_t width, uint32_t val);
int devmem_clr_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_set_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit);
int devmem_get_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit);

#endif /* __DEVMEM_H__ */
