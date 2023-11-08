#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "devmem.h"

#define DEVMEM "/dev/mem"

devmem_t *devmem_open(void *addr)
{
	long page_size;
	devmem_t *devmem = NULL;

	if (addr == NULL)
		return NULL;

	devmem = malloc(sizeof(devmem_t));
	if (!devmem) {
		perror("malloc devmem");
		return NULL;
	}

	devmem->addr = addr;
	devmem->map_size = page_size = sysconf(_SC_PAGE_SIZE);
	if (page_size <= 0) {
		perror("sysconf failed");
		return NULL;
	}

	devmem->fd = open(DEVMEM, O_SYNC | O_RDWR);
	if (devmem->fd < 0) {
		perror("open /dev/mem failed");
		return NULL;
	}

	/**
	 * Here map one page (4096 Bytes)
	 * The map offset must be a multiple of the page size
	 *
	 *  page_size      -> 0x1000 (4096)
	 *  page_size-1    -> 0x0FFF
	 *  ~(page_size-1) -> 0xF000
	 */
	devmem->vt_base_addr = mmap(NULL,
					devmem->map_size,
					PROT_READ | PROT_WRITE,
					MAP_SHARED,
					devmem->fd,
					(off_t)devmem->addr & ~(page_size-1));
	if (devmem->vt_base_addr == MAP_FAILED)
		goto err;

	devmem->vt_addr = devmem->vt_base_addr + ((off_t)devmem->addr & (page_size-1));

	return devmem;

err:
	if (devmem->fd > 0)
		close(devmem->fd);

	return NULL;
}

int devmem_close(devmem_t *devmem)
{
	int ret;

	if (!devmem)
		return -1;

	ret = munmap(devmem->vt_base_addr, devmem->map_size);
	if (ret) {
		perror("munmap failed");
		return -1;
	}

	ret = close(devmem->fd);
	if (ret) {
		perror("close /dev/mem failed");
		return -1;
	}

	return 0;
}

int devmem_read(devmem_t *devmem, uint8_t offset, uint8_t width, uint32_t *val)
{
	if (!devmem || !val)
		return -1;

	switch (width) {
		case 8:
			*val = *((uint8_t *)devmem->vt_addr + offset);
			break;
		case 16:
			*val = *((uint16_t *)devmem->vt_addr + offset);
			break;
		case 32:
			*val = *(uint32_t *)(devmem->vt_addr + offset);
			break;
		default:
			fprintf(stderr, "Read unsupport width: %d\n",width);
			return -1;
			break;
	}
	printf("[DEBUG-%s] addr=0x%08lx, val=0x%08x\n", __func__, (uint64_t)devmem->addr + offset, *val);
	return 0;
}

int devmem_write(devmem_t *devmem, uint8_t offset, uint8_t width, uint32_t val)
{
	if (!devmem)
		return -1;

	switch (width) {
		case 8:
			*(uint8_t *)(devmem->vt_addr + offset) = val;
			break;
		case 16:
			*(uint16_t *)(devmem->vt_addr + offset) = val;
			break;
		case 32:
			*(uint32_t *)(devmem->vt_addr + offset) = val;
			break;
		default:
			fprintf(stderr, "Write unsupport width: %d\n", width);
			return -1;
			break;
	}

	printf("[DEBUG-%s] addr=0x%08lx, val=0x%08x\n", __func__, (uint64_t)devmem->addr + offset, val);
	return 0;
}

int devmem_clr_bit(devmem_t *devmem, uint8_t offset, uint8_t width, uint8_t bit)
{
	uint32_t val;

	if (!devmem)
		return -1;

	if (0 > devmem_read(devmem, offset, width, &val))
		return -2;

	val &= ~(1 << bit);

	if (0 > devmem_write(devmem, offset, width, val))
		return -3;

	return 0;
}

int devmem_set_bit(devmem_t *devmem, uint8_t offset, uint8_t width, uint8_t bit)
{
	uint32_t val;

	if (!devmem)
		return -1;

	if (0 > devmem_read(devmem, offset, width, &val))
		return -2;

	val |= (1 << bit);

	if (0 > devmem_write(devmem, offset, width, val))
		return -3;

	return 0;
}

int devmem_get_bit(devmem_t *devmem, uint8_t offset, uint8_t width, uint8_t bit)
{
	uint32_t val;

	if (!devmem)
		return -1;

	if (0 > devmem_read(devmem, offset, width, &val))
		return -1;

	return ((val >> bit) & 0x1);
}

int devmem_read2(void *addr, uint8_t offset, uint8_t width, uint32_t *val)
{
	int ret;
	devmem_t *devmem;

	devmem = devmem_open(addr);
	if (!devmem)
		return -1;

	ret = devmem_read(devmem, offset, width, val);
	if (ret < 0)
		goto err;

	return devmem_close(devmem);

err:
	devmem_close(devmem);
	return -2;
}

int devmem_write2(void *addr, uint8_t offset, uint8_t width, uint32_t val)
{
	int ret;
	devmem_t *devmem;

	devmem = devmem_open(addr);
	if (!devmem)
		return -1;

	ret = devmem_write(devmem, offset, width, val);
	if (ret < 0)
		goto err;

	devmem_close(devmem);
	return 0;

err:
	devmem_close(devmem);
	return -2;
}

int devmem_clr_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	devmem_t *devmem;

	devmem = devmem_open(addr);
	if (!devmem)
		return -1;

	if (0 > devmem_clr_bit(devmem, offset, width, bit))
		goto err;

	devmem_close(devmem);
	return 0;

err:
	devmem_close(devmem);
	return -2;
}

int devmem_set_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	devmem_t *devmem;

	devmem = devmem_open(addr);
	if (!devmem)
		return -1;

	if (0 > devmem_set_bit(devmem, offset, width, bit))
		goto err;

	devmem_close(devmem);
	return 0;

err:
	devmem_close(devmem);
	return -2;
}

int devmem_get_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	devmem_t *devmem;
	int val;

	devmem = devmem_open(addr);
	if (!devmem)
		return -1;

	val = devmem_get_bit(devmem, offset, width, bit);
	if (val < 0)
		goto err;

	devmem_close(devmem);
	return val;

err:
	devmem_close(devmem);
	return -2;
}
