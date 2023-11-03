#include <stdio.h>
#include <stdint.h>
#include "devmem.h"

#define SET_BIT(val, bit) \
	(val|= (1 << bit))
#define CLEAR_BIT(x, bit) \
	(val &= ~(1 << bit))

void *devmem_open(void)
{
	return NULL;
}

int devmem_close(void *virt_addr)
{
	return 0;
}

int devmem_read(void *virt_addr, uint8_t len, uint32_t *val)
{
	return 0;
}

int devmem_write(void *virt_addr, uint8_t len, uint32_t val)
{
	return 0;
}
