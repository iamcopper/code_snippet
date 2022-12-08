#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "inc.h"

#define BUF_SIZE 64

int main(int argc, char *argv[])
{
	int sockfd, ret;

	/* TCP Socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(IP_LOOPBACK);

	/* Connect to TCP Server */
	ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	if (ret < 0) {
		perror("connect");
		close(sockfd);
		return -1;
	}

	/* Detail: man 7 socket */
	/* Set clientfd SEND/RECV timeout */
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

	char send_buf[BUF_SIZE] = {0};
	char recv_buf[BUF_SIZE] = {0};

	while(1) {
		fgets(send_buf, BUF_SIZE, stdin);

		ret = send(sockfd, &send_buf, strlen(send_buf), 0);
		if (ret < 0) {
			perror("send");
			break;
		}
		printf(">>>[%d] bytes sent.\n", ret);

		if (strncmp(send_buf, "quit", 4) == 0
			|| strncmp(send_buf, "exit", 4) == 0)
			break;

		memset(recv_buf, 0, sizeof(recv_buf));
		ret = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
		if (ret < 0) {
			perror("recv");
			break;
		}

		printf("<<<[%d] bytes recevied: %s\n", ret, recv_buf);
	}

	close(sockfd);
	return 0;
}
