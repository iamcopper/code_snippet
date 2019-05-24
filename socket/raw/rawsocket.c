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

#define IF_NAME  "eth0"
#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
	int sockfd, ret;
	struct ifreq ifr;
	struct sockaddr_ll sl;

	/* RAW socket */
	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}

	/* Set NIC as promisc mode */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, IF_NAME, IFNAMSIZ);
	ret = ioctl(sockfd, SIOCGIFFLAGS, &ifr);
	if (ret < 0) {
		perror("ioctl1");
		close(sockfd);
		return -1;
	}

	ifr.ifr_flags |= IFF_PROMISC;
	ret = ioctl(sockfd, SIOCSIFFLAGS, &ifr);
	if (ret < 0) {
		perror("ioctl1");
		close(sockfd);
		return -1;
	}

	/* Define local network interface as IF_NAME */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, IF_NAME, IFNAMSIZ);
	ret = ioctl(sockfd, SIOCGIFINDEX, &ifr);
	if (ret < 0) {
		perror("ioctl3");
		close(sockfd);
		return -1;
	}

	memset(&sl, 0, sizeof(struct sockaddr_ll));
	sl.sll_ifindex = ifr.ifr_ifindex;
	sl.sll_family = PF_PACKET;
	sl.sll_protocol = htons(ETH_P_ALL);

	ret = bind(sockfd, (struct sockaddr *)&sl, sizeof(struct sockaddr_ll));
	if (ret < 0) {
		perror("bind");
		close(sockfd);
		return -1;
	}

	uint8_t buf[BUF_SIZE] = {0};
	uint8_t expect_char = 0;
	unsigned int i = 0, lost_packets = 0;

	while (1) {
		ret = recv(sockfd, buf, BUF_SIZE, 0);
		if (ret <= 0)
			continue;

		printf(">>>>> [%10u] Packet Received, buf[0]=0x%02x\n", i, buf[0]);

		/* Check whether packet lost */
		if (buf[0] != expect_char) {
			lost_packets++;
			fprintf(stderr, "[%10u] Packet lost %u. buf[0]=0x%02x, expect=0x%02x\n", i, lost_packets, buf[0], expect_char);
		}

		/* Update expected next character */
		expect_char = buf[0] + 1;

		/* Send data back */
		ret = send(sockfd, buf, ret, 0);
		printf("<<<<< [%10u] Packet Send back, buf[0]=0x%02x\n", i, buf[0]);

		i++;
	}

	close(sockfd);

	return 0;
}
