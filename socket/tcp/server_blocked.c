#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "inc.h"

#define BUF_SIZE  1024
#define IP_LISTEN IP_ANY

int main(int argc, char *argv[])
{
	int sockfd, ret;

	/* TCP Socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}

	/* Set IP and PORT to BIND */
	struct sockaddr_in servaddr;
	servaddr.sin_family = PF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(IP_LISTEN);
	//inet_aton(IP_LISTEN, &(servaddr.sin_addr));

	/* Eliminates "Address already in use" error form bind. <Computer System 2nd> P629 */
	int on = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("setsockopt");
		goto err;
	}

	/* Bind Server IP and PORT */
	ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));
	if (ret < 0) {
		perror("bind");
		goto err;
	}

	printf("[INFO] Now TCP server listen at host:%s port:%d\n", IP_LISTEN, PORT);
	ret = listen(sockfd, 1024);
	if (ret < 0) {
		perror("listen");
		goto err;
	}

	char buf[BUF_SIZE] = {0};

	struct sockaddr_in clientaddr;
	socklen_t clientlen;
	char clientaddr_str[16];
	uint16_t clientport;
	int clientfd = 0;
	int recvlen;

	/* Only one client request can be processed at the same time */
	while(1) {
		printf("[INFO] Waiting for new connection ...\n");

		/* <UNIX网络编程:卷1> Ch3.3, 4.6, 8.2: Value-Result参数 */
		clientlen = sizeof(clientaddr);
		clientfd = accept(sockfd, (struct sockaddr *)&clientaddr, &clientlen);
		if (clientfd < 0) {
			perror("accept");
			continue;
		}

		/* Detail: man 7 socket */
		/* Set clientfd SEND/RECV timeout */
		struct timeval tv;
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
		setsockopt(clientfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);

		memset(clientaddr_str, 0, sizeof(clientaddr_str));
		strncpy(clientaddr_str, inet_ntoa(clientaddr.sin_addr), sizeof(clientaddr_str));
		clientport = ntohs(clientaddr.sin_port);

		printf("<<< Accept a new TCP connection from host:%s port:%d\n", clientaddr_str, clientport);

		while(1) {
			ret = recv(clientfd, buf, BUF_SIZE, 0);
			if (ret <= 0) {
				perror("recv");
				break;
			}

			printf("<<<[%s:%d] %d bytes received: %s\n", clientaddr_str, clientport, ret, buf);
			recvlen = ret;
			if (strncmp(buf, "quit", 4) == 0
				|| strncmp(buf, "exit", 4) == 0)
				break;

			ret = send(clientfd, buf, recvlen, 0);
			if (ret <= 0) {
				perror("send");
				break;
			}
			printf(">>>[%s:%d] %d bytes sent back.\n", clientaddr_str, clientport, ret);
		}
		close(clientfd);
		printf("\nConnection terminated from host:%s port:%d\n\n", clientaddr_str, clientport);
	}

	close(sockfd);
	return 0;

err:
	close(sockfd);
	return -1;
}
