#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <net/if.h>

int main(int argc, char *argv[])
{
	int sockfd = 0;
	int ret = 0;
	struct ifreq ifreq;

	if (argc < 2) {
		printf("Usage: %s <net interface>\n", argv[0]);
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror ("socket");
		return -1;
	}

	memset(&ifreq, 0, sizeof(struct ifreq));
	strcpy(ifreq.ifr_name, argv[1]);
	ret = ioctl(sockfd, SIOCGIFHWADDR, &ifreq);
	if (ret < 0) {
		perror ("ioctl");
		goto end;
	}

	printf("NET Interface: %s\n", argv[1]);
	printf("MAC Address  : %x:%x:%x:%x:%x:%x\n",
			(unsigned char) ifreq.ifr_hwaddr.sa_data[0],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[1],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[2],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[3],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[4],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[5]);

end:
	close(sockfd);
	return ret;
}
