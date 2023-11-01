/**
 * Access PCIe memory space from sysfs interface without PCIe driver
 *
 * This demo is to access the PCIe BAR1 reg 10,11,12,13,14 in FPGA to
 * perform CAN send and receive. The CAN communication is implemented by FPGA.
 * It can access the PCIe memory space from
 * sysfs, and run without PCIe driver.
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define SYSFS_FPGA_PATH    "/sys/bus/pci/devices/0000:04:00.0/"
#define SYSFS_FPGA_RES     SYSFS_FPGA_PATH "resource"
#define SYSFS_FPGA_BAR0    SYSFS_FPGA_PATH "resource0"
#define SYSFS_FPGA_BAR1    SYSFS_FPGA_PATH "resource1"
#define SYSFS_FPGA_BAR2    SYSFS_FPGA_PATH "resource2"
#define SYSFS_FPGA_BAR3    SYSFS_FPGA_PATH "resource3"

#define BAR0   0
#define BAR1   1
#define BAR2   2
#define BAR3   3

#define CAN_CTL_STOP   0
#define CAN_CTL_SEND   1
#define CAN_CTL_RECV   2

#define BUF_SIZE 256

uint32_t *get_pci_bar_addr(int barno)
{
	/**
	 * Getting pci bar addr from "resource" node in sysfs
	 * Each line in it stands for a PCI BAR address info,
	 * eg: the first line describe BAR0 address info, the second line describe BAR1...
	 */
	FILE *fp = fopen(SYSFS_FPGA_RES, "r");
	if (fp < 0) {
		perror("fopen");
		return NULL;
	}

	/* Get the specific BAR address info from the specific line */
	char buf[BUF_SIZE] = {0};
	while (barno-- >= 0) {
		memset(buf, 0, sizeof(buf));
		fgets(buf, sizeof(buf), fp);
	}

	/* Parse the base address from the first value in buffer */
	char *first_blank = strchr(buf, ' ');
	if (!first_blank) {
		fprintf(stderr, "strchr() error.");;
		fclose(fp);
		return NULL;
	}
	*first_blank = '\0';

	char *ptr = NULL;
	long long int addr = strtoll(buf, &ptr, 16);
	fclose(fp);

	return (uint32_t *)addr;
}

void can_send(uint32_t *bar_addr, uint32_t id, uint8_t data[])
{
	uint32_t *ctl_reg = bar_addr + 0x10;
	uint32_t *send_reg1 = bar_addr + 0x11;
	uint32_t *send_reg2 = bar_addr + 0x12;


	/* Little Endian */
	*send_reg1 = data[0] << 0 | data[1] << 8 | data[2] << 16 | data[3] << 24;
	*send_reg2 = data[4] << 0 | data[5] << 8 | data[6] << 16 | data[7] << 24;
	printf(">>> Send: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
				data[0], data[1], data[2], data[3],
				data[4], data[5], data[6], data[7]);

	*ctl_reg = (CAN_CTL_SEND << 12) | (id & 0xFFF);
	*ctl_reg = (CAN_CTL_STOP << 12) | (*ctl_reg & 0xFFF);
}

void can_recv(uint32_t *bar_addr, uint32_t id, uint8_t data[])
{
	uint32_t *ctl_reg = bar_addr + 0x10;
	uint32_t *recv_reg1 = bar_addr + 0x13;
	uint32_t *recv_reg2 = bar_addr + 0x14;

	*ctl_reg = (CAN_CTL_RECV << 12) | (id & 0xFFF);

	/* Little Endian */
	int i = 1;
	while(i++ < 10) {
		sleep(1);
		data[0] = (*recv_reg1 >> 0 ) & 0xFF;
		data[1] = (*recv_reg1 >> 8 ) & 0xFF;
		data[2] = (*recv_reg1 >> 16) & 0xFF;
		data[3] = (*recv_reg1 >> 24) & 0xFF;
		data[4] = (*recv_reg2 >> 0 ) & 0xFF;
		data[5] = (*recv_reg2 >> 8 ) & 0xFF;
		data[6] = (*recv_reg2 >> 16) & 0xFF;
		data[7] = (*recv_reg2 >> 24) & 0xFF;
		printf(">>> [loop=%d] Recv: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
				i, data[0], data[1], data[2], data[3],
				data[4], data[5], data[6], data[7]);
	}

	*ctl_reg = (CAN_CTL_STOP << 12) | (*ctl_reg & 0xFFF);
}

int main(int argc, char *argv[])
{
	uint32_t *bar1_addr_orig = get_pci_bar_addr(BAR1);
	if (!bar1_addr_orig) {
		fprintf(stderr, "Get PCI Bar address failed!\n");
		return -1;
	}

	int fd = open(SYSFS_FPGA_BAR1, O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("open");
		return -1;
	}
	size_t size = 16;

	uint32_t *bar1_addr_mapped = (uint32_t *)mmap(bar1_addr_orig,
						size,
						PROT_READ | PROT_WRITE,
						MAP_SHARED,
						fd,
						0);
	if (!bar1_addr_mapped) {
		perror("mmap");
		close(fd);
		return -1;
	}

#if 0
	uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	can_send(bar1_addr_mapped, 0x123, data);
	data[6] = 0x77;
	data[7] = 0x88;
	can_send(bar1_addr_mapped, 0x123, data);
#else
	uint8_t data[8] = {0};
	can_recv(bar1_addr_mapped, 0x123, data);
#endif

	munmap(bar1_addr_mapped, size);
	close(fd);

	return 0;
}
