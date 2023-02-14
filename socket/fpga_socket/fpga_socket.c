#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include "fpga_socket.h"

#define __DEBUG__

#ifdef __DEBUG__
#define dprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define dperror(str)      perror(str)
#else
#define dprintf(fmt, ...)
#define dperror(str)
#endif

#define MTU_MAX                2000
#define MTU_MIN                64

#define DST_MAC_POS            0
#define SRC_MAC_POS            6
#define LINK_DATA_TYPE_POS     12
#define PAYLOAD_POS            14

#define CMD_POS                14
#define ADDR_POS               15
#define DATA_LEN_POS           19
#define DATA_POS               23

#define MAC_LEN                ((SRC_MAC_POS) - (DST_MAC_POS))

#define LINK_HEADER_LEN        ((CMD_POS) - (DST_MAC_POS))
#define LINK_TRAILER_LEN       4
#define DATA_HEADER_LEN        ((DATA_POS) - (DST_MAC_POS))
#define DATA_LEN_MAX           ((MTU_MAX) - (DATA_HEADER_LEN) - (LINK_TRAILER_LEN))
#define DATA_LEN_MIN           ((MTU_MIN) - (DATA_HEADER_LEN) - (LINK_TRAILER_LEN))

const uint8_t fpga_mac[] = {0xEB, 0x90, 0xEB, 0x90, 0xEB, 0x90};

static uint32_t caculate_chksum(uint8_t *data, size_t len)
{
	int i = 0;
	uint32_t chksum = 0;

	for(i = 0; i < len; i++)
		chksum += data[i];

	return chksum;
}

static ssize_t do_write_fpga_ram(int fd, const uint8_t *buf, size_t len, int flags)
{
	static uint8_t wbuf[MTU_MAX] = {0};

	uint32_t addr, datalen;
	uint32_t waddr, wlen, padlen;
	uint32_t chksum;
	size_t ret, totalwrite = 0;

	addr = buf[ADDR_POS] & 0xFF;
	addr |= (buf[ADDR_POS + 1] & 0xFF) << 8;
	addr |= (buf[ADDR_POS + 2] & 0xFF) << 16;
	addr |= (buf[ADDR_POS + 3] & 0xFF) << 24;

	datalen = buf[DATA_LEN_POS] & 0xFF;
	datalen |= (buf[DATA_LEN_POS + 1] & 0xFF) << 8;
	datalen |= (buf[DATA_LEN_POS + 2] & 0xFF) << 16;
	datalen |= (buf[DATA_LEN_POS + 3] & 0xFF) << 24;

	if (len != datalen + DATA_HEADER_LEN + LINK_TRAILER_LEN) {
		dprintf("[%s:%d] Data format error.\n", __func__, __LINE__);
		return -1;
	}

	memset(wbuf, 0, sizeof(wbuf));
	memcpy(wbuf, buf, LINK_HEADER_LEN);
	wbuf[CMD_POS] = buf[CMD_POS];

	while (datalen > totalwrite) {
		if (datalen - totalwrite >= DATA_LEN_MAX) {
			wlen = DATA_LEN_MAX;
			padlen = 0;
		} else if (datalen - totalwrite <= DATA_LEN_MIN) {
			wlen = datalen - totalwrite;
			padlen = DATA_LEN_MIN - (datalen - totalwrite);
		} else {
			wlen = datalen - totalwrite;
			padlen = 0;
		}
	
		/* Caculate Data Address for every write */
		waddr = addr + totalwrite;
		wbuf[ADDR_POS] = waddr & 0xFF;
		wbuf[ADDR_POS + 1] = (waddr >> 8) & 0xFF;
		wbuf[ADDR_POS + 2] = (waddr >> 16) & 0xFF;
		wbuf[ADDR_POS + 3] = (waddr >> 24) & 0xFF;

		/* Caculate Data Length for every write */
		wbuf[DATA_LEN_POS] = wlen & 0xFF;
		wbuf[DATA_LEN_POS + 1] = (wlen >> 8) & 0xFF;
		wbuf[DATA_LEN_POS + 2] = (wlen >> 16) & 0xFF;
		wbuf[DATA_LEN_POS + 3] = (wlen >> 24) & 0xFF;

		memcpy(wbuf, buf + totalwrite, wlen);
		memset(wbuf, 0, padlen);
		
		/* Update checksum */
		chksum = caculate_chksum(wbuf, DATA_HEADER_LEN + wlen + padlen);
		wbuf[DATA_LEN_POS + wlen + padlen] = chksum & 0xFF;
		wbuf[DATA_LEN_POS + wlen + padlen + 1] = (chksum >> 8) & 0xFF;
		wbuf[DATA_LEN_POS + wlen + padlen + 2] = (chksum >> 16) & 0xFF;
		wbuf[DATA_LEN_POS + wlen + padlen + 3] = (chksum >> 24) & 0xFF;

		ret = send(fd, wbuf, DATA_HEADER_LEN + wlen + padlen + LINK_TRAILER_LEN, flags);
		if (ret < 0) {
			dprintf("[%s:%d] send error, ret=%ld: %s.\n", __func__, __LINE__, ret, strerror(errno));
			return ret;
		}

		totalwrite += ret;
	}

	return totalwrite;
}

int fpga_open(const char *if_name)
{
	int fd, ret;
	struct ifreq ifr;
	struct sockaddr_ll sl;

	if (if_name == NULL)
		return -1;

	/* RAW socket */
	fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0) {
		dperror("socket");
		return -1;
	}

	/* Set NIC as promisc mode */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
	ret = ioctl(fd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		dperror("ioctl1");
		close(fd);
		return -1;
	}

	ifr.ifr_flags |= IFF_PROMISC;
	ret = ioctl(fd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		dperror("ioctl2");
		close(fd);
		return -1;
	}

	/* Define local network interface as if_name */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, if_name, IFNAMSIZ);
	ret = ioctl(fd, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		dperror("ioctl3");
		close(fd);
		return -1;
	}

	memset(&sl, 0, sizeof(struct sockaddr_ll));
	sl.sll_ifindex = ifr.ifr_ifindex;
	sl.sll_family = PF_PACKET;
	sl.sll_protocol = htons(ETH_P_ALL);

	ret = bind(fd, (struct sockaddr *)&sl, sizeof(struct sockaddr_ll));
	if (ret < 0) {
		dperror("bind");
		close(fd);
		return -1;
	}

	return fd;
}

void fpga_close(int fd)
{
	close(fd);
}

ssize_t fpga_write(int fd, const uint8_t *buf, size_t len, int flags)
{
	ssize_t ret= -1;

	if (memcmp(&buf[DST_MAC_POS], fpga_mac, MAC_LEN) == 0) {
		ret = do_write_fpga_ram(fd, buf, len, flags);
	} else {
		if (len > MTU_MAX || len < MTU_MIN) {
			dprintf("Parameter Error: Write length(%ld) should between %d and %d.\n", len, MTU_MIN, MTU_MAX);
			return -1;
		}

		ret = send(fd, buf, len, flags);
	}

	return ret;
}

ssize_t fpga_read(int fd, uint8_t *buf, size_t len, int flags)
{
	if (len < MTU_MAX) {
		dprintf("Parameter Error: Read buffer length(%ld) should not lower than %d.\n", len, MTU_MAX);
		return -1;
	}

	return recv(fd, buf, len, flags);
}
