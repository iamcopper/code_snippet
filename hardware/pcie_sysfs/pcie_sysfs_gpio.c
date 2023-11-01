/**
 * Access PCIe memory space from sysfs interface without PCIe driver
 *
 * This demo is to access the PCIe BAR2 reg 3 in FPGA to control
 * the 2 LED indirectly. It can access the PCIe memory space from
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

#define LED1   0
#define LED2   1

#define BUF_SIZE 128

uint32_t *get_pci_bar_addr(int barno)
{
	/**
	 * Getting pci bar addr from "resource" node in sysfs
	 * Each line in it stands for the PCI BAR address info,
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

void led_on(uint32_t *led_ctl_reg, int ledno)
{
	*led_ctl_reg |= (1 << ledno);
}

void led_off(uint32_t *led_ctl_reg, int ledno)
{
	*led_ctl_reg &= ~(1 << ledno);
}

int main(int argc, char *argv[])
{
	uint32_t *bar2_addr_orig = get_pci_bar_addr(BAR2);
	if (!bar2_addr_orig) {
		printf("get bar2_addr failed.\n");
		return -1;
	}

	int fd = open(SYSFS_FPGA_BAR2, O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("open");
		return -1;
	}
	size_t size = 8;


	uint32_t *bar2_addr_mapped = (uint32_t *)mmap(bar2_addr_orig,
						size,
						PROT_READ | PROT_WRITE,
						MAP_SHARED,
						fd,
						0);
	if (!bar2_addr_mapped) {
		perror("mmap");
		close(fd);
		return -1;
	}

	uint32_t *led_ctl_reg = bar2_addr_mapped + 3;

	int n = 3;
	while(n-- > 0) {
		led_on(led_ctl_reg, LED1);
		sleep(1);
		led_on(led_ctl_reg, LED2);
		sleep(1);
		led_off(led_ctl_reg, LED1);
		sleep(1);
		led_off(led_ctl_reg, LED2);
		sleep(1);
	}

	munmap(bar2_addr_mapped, size);
	close(fd);

	return 0;
}
