#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "inc.h"

int main(int argc, char *argv[])
{
	int sockfd, ret;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}

	const int on = 1;
	ret = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	if (ret < 0) {
		perror("setsockopt");
		return -1;
	}

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(IP_BROADCAST);

	ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	if (ret < 0) {
		perror("connect");
		close(sockfd);
		return -1;
	}

	uint8_t buf = 0;

	while(getchar() != 'q') {
		ret = send(sockfd, &buf, sizeof(uint8_t), 0);
		if (ret < 0) {
			perror("send");
		}
		printf("Send %d bytes, data: 0x%x.\n", ret, buf);
		buf++;
	}

	close(sockfd);

	return 0;
}
