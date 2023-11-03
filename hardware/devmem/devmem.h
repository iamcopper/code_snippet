#ifndef __DEVMEM_H__
#define __DEVMEM_H__

#include <stdint.h>

void *devmem_open(void);
int devmem_close(void *addr);

int devmem_read(void *virt_addr, uint8_t len, uint32_t *val);
int devmem_write(void *virt_addr, uint8_t len, uint32_t val);

#endif /* __DEVMEM_H__ */
