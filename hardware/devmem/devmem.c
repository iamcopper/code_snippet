#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "devmem.h"

#define DEVMEM "/dev/mem"

struct devmem {
	void *addr;
	void *virt_base_addr;
	void *virt_addr;
	size_t map_size;
	int fd;
};
static struct devmem vm = {
	.fd = -1,
};

int devmem_open(void *addr)
{
	long page_size;

	if (addr == NULL)
		return -1;

	if (vm.fd != -1)
		return -1;

	vm.addr = addr;
	vm.map_size = page_size = sysconf(_SC_PAGE_SIZE);
	if (page_size <= 0) {
		perror("sysconf failed");
		return -1;
	}

	vm.fd = open(DEVMEM, O_SYNC | O_RDWR);
	if (vm.fd < 0) {
		perror("open /dev/mem failed");
		return -1;
	}

	/**
	 * Here map one page (4096 Bytes)
	 * The map offset must be a multiple of the page size
	 *
	 *  page_size      -> 0x1000 (4096)
	 *  page_size-1    -> 0x0FFF
	 *  ~(page_size-1) -> 0xF000
	 */
	vm.virt_base_addr = mmap(NULL,
					vm.map_size,
					PROT_READ | PROT_WRITE,
					MAP_SHARED,
					vm.fd,
					(off_t)vm.addr & ~(page_size-1));
	if (vm.virt_base_addr == MAP_FAILED)
		goto err;

	vm.virt_addr = vm.virt_base_addr + ((off_t)vm.addr & (page_size-1));

	return vm.fd;

err:
	if (vm.fd > 0) {
		close(vm.fd);
		vm.fd = -1;
	}

	return -1;
}

int devmem_close(int fd)
{
	int ret;

	if (fd <= 0 || fd != vm.fd)
		return -1;

	ret = munmap(vm.virt_base_addr, vm.map_size);
	if (ret) {
		perror("munmap failed");
		return -1;
	}

	ret = close(vm.fd);
	if (ret) {
		perror("close /dev/mem failed");
		return -1;
	}

	vm.fd = -1;
	return 0;
}

int devmem_read(int fd, uint8_t offset, uint8_t width, uint32_t *val)
{
	int ret = 0;

	if (fd <= 0 || fd != vm.fd)
		return -1;

	switch (width) {
		case 8:
			*val = *((uint8_t *)vm.virt_addr + offset);
			break;
		case 16:
			*val = *((uint16_t *)vm.virt_addr + offset);
			break;
		case 32:
			*val = *(uint32_t *)(vm.virt_addr + offset);
			break;
		default:
			fprintf(stderr, "Read unsupport width: %d\n",width);
			ret = -1;
			break;
	}

	return ret;
}

int devmem_write(int fd, uint8_t offset, uint8_t width, uint32_t val)
{
	int ret = 0;

	if (fd <= 0 || fd != vm.fd)
		return -1;

	switch (width) {
		case 8:
			*(uint8_t *)(vm.virt_addr + offset) = val;
			break;
		case 16:
			*(uint16_t *)(vm.virt_addr + offset) = val;
			break;
		case 32:
			*(uint32_t *)(vm.virt_addr + offset) = val;
			break;
		default:
			fprintf(stderr, "Write unsupport width: %d\n", width);
			ret = -1;
			break;
	}

	return ret;
}

int devmem_clr_bit(int fd, uint8_t offset, uint8_t width, uint8_t bit)
{
	uint32_t val;

	if (fd <= 0 || fd != vm.fd)
		return -1;

	if (0 > devmem_read(fd, offset, width, &val))
		return -1;

	val &= ~(1 << bit);

	if (0 > devmem_write(fd, offset, width, val))
		return -1;

	return 0;
}

int devmem_set_bit(int fd, uint8_t offset, uint8_t width, uint8_t bit)
{
	uint32_t val;

	if (fd <= 0 || fd != vm.fd)
		return -1;

	if (0 > devmem_read(fd, offset, width, &val))
		return -1;

	val |= (1 << bit);

	if (0 > devmem_write(fd, offset, width, val))
		return -1;

	return 0;
}

int devmem_get_bit(int fd, uint8_t offset, uint8_t width, uint8_t bit)
{
	uint32_t val;

	if (fd <= 0 || fd != vm.fd)
		return -1;

	if (0 > devmem_read(fd, offset, width, &val))
		return -1;

	return ((val >> bit) & 0x1);
}

int devmem_read2(void *addr, uint8_t offset, uint8_t width, uint32_t *val)
{
	int fd, ret;

	fd = devmem_open(addr);
	if (fd <= 0)
		return -1;

	ret = devmem_read(fd, offset, width, val);
	if (ret < 0)
		goto err;

	return devmem_close(fd);

err:
	devmem_close(fd);
	return -2;
}

int devmem_write2(void *addr, uint8_t offset, uint8_t width, uint32_t val)
{
	int fd, ret;

	fd = devmem_open(addr);
	if (fd <= 0)
		return -1;

	ret = devmem_write(fd, offset, width, val);
	if (ret < 0)
		goto err;

	devmem_close(fd);
	return 0;

err:
	devmem_close(fd);
	return -2;
}

int devmem_clr_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	int fd;

	fd = devmem_open(addr);
	if (fd <= 0)
		return -1;

	if (0 > devmem_clr_bit(fd, offset, width, bit))
		goto err;

	devmem_close(fd);
	return 0;

err:
	devmem_close(fd);
	return -1;
}

int devmem_set_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	int fd;

	fd = devmem_open(addr);
	if (fd <= 0)
		return -1;

	if (0 > devmem_set_bit(fd, offset, width, bit))
		goto err;

	devmem_close(fd);
	return 0;

err:
	devmem_close(fd);
	return -1;
}

int devmem_get_bit2(void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	int fd;
	int val;

	fd = devmem_open(addr);
	if (fd <= 0)
		return -1;

	val = devmem_get_bit(fd, offset, width, bit);
	if (val < 0)
		goto err;

	devmem_close(fd);
	return val;

err:
	devmem_close(fd);
	return -1;
}

int devmem_read_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint32_t *val)
{
	if (vm.fd > 0)
		return -1;

	if (fd > 0 && vm.fd == fd)
		return devmem_read(fd, offset, width, val);

	return devmem_read2(addr, offset, width, val);
}

int devmem_write_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint32_t val)
{
	if (vm.fd > 0)
		return -1;

	if (fd > 0 && vm.fd == fd)
		return devmem_write(fd, offset, width, val);

	return devmem_write2(addr, offset, width, val);
}

int devmem_clr_bit_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	if (vm.fd > 0)
		return -1;

	if (fd > 0 && vm.fd == fd)
		return devmem_clr_bit(fd, offset, width, bit);

	return devmem_clr_bit2(addr, offset, width, bit);
}

int devmem_set_bit_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	if (vm.fd > 0)
		return -1;

	if (fd > 0 && vm.fd == fd)
		return devmem_set_bit(fd, offset, width, bit);

	return devmem_set_bit2(addr, offset, width, bit);
}

int devmem_get_bit_opt(int fd, void *addr, uint8_t offset, uint8_t width, uint8_t bit)
{
	if (vm.fd > 0)
		return -1;

	if (fd > 0 && vm.fd == fd)
		return devmem_get_bit(fd, offset, width, bit);

	return devmem_get_bit2(addr, offset, width, bit);
}
