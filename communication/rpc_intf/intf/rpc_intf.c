/**
 * \file
 *
 * The RPC interface implement
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "log.h"
#include "utils.h"
#include "rpc_intf.h"

static void rpc_msg_serialize(uint8_t *buf, size_t *buf_len, rpc_msg_t *msg);
static int rpc_msg_deserialize(rpc_msg_t *msg, uint8_t *buf, size_t buf_len);
static void print_msg(rpc_msg_t *msg);

/*
 * UDP socket
 *
 * Client: connect the specified server addr during open,
 * and then all communications(sendto/recvfrom) with the server.
 *
 * Server: bind own address, and need to distinguish the message
 * from which client and to which client.
 */
rpc_intf_t *rpc_intf_open(int oflag, rpc_addr_t *servaddr)
{
	rpc_intf_t *intf = NULL;

	intf = malloc(sizeof(rpc_intf_t));
	if (intf == NULL)
	{
		lperror(LOG_ERR, "malloc");
		return NULL;
	}

	bzero(intf, sizeof(rpc_intf_t));

	intf->oflag = oflag;
	intf->fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (intf->fd < 0)
	{
		lperror(LOG_ERR, "socket");
		free(intf);
		return NULL;
	}

	if (intf->oflag == OFLAG_SERVER)
		bind(intf->fd, &(servaddr->addr), servaddr->len);
	else
		connect(intf->fd, &(servaddr->addr), servaddr->len);

	return intf;
}

int rpc_intf_sendto(rpc_intf_t *intf, rpc_msg_t *msg, rpc_addr_t *dst)
{
	static uint8_t buf[RPC_BUFFER_SIZE] = {0};
	size_t buf_len = 0;
	int ret = E_SEND;

	rpc_msg_serialize(buf, &buf_len, msg);

	if (intf->oflag & OFLAG_SERVER)
		ret = sendto(intf->fd, buf, buf_len, 0, &(dst->addr), dst->len);
	else
		ret = send(intf->fd, buf, buf_len, 0);

	if (ret < 0)
		return E_SEND;

	printf("\nsend msg: \n");
	print_msg(msg);

	return ret;
}

int rpc_intf_recvfrom(rpc_intf_t *intf, rpc_msg_t *msg, rpc_addr_t *src)
{
	static uint8_t buf[RPC_BUFFER_SIZE] = {0};
	size_t buf_len = 0;
	int ret = E_RECV;

	if (intf->oflag & OFLAG_SERVER)
		ret = recvfrom(intf->fd, buf, RPC_BUFFER_SIZE, 0, &(src->addr), &(src->len));
	else
		ret = recv(intf->fd, buf, RPC_BUFFER_SIZE, 0);

	if (ret < 0)
		return E_RECV;

	buf_len = ret;
	if (rpc_msg_deserialize(msg, buf, buf_len) != E_OK)
		return E_CHECKSUM;

	printf("\nrecv msg: \n");
	print_msg(msg);

	return ret;
}

int rpc_intf_close(rpc_intf_t *intf)
{
	close(intf->fd);

	free(intf);
	intf = NULL;

	return E_OK;
}

int rpc_addr_pack(rpc_addr_t *rpcaddr, uint32_t host, uint16_t port)
{
	struct sockaddr_in *addr = (struct sockaddr_in *)&(rpcaddr->addr);

	bzero(rpcaddr, sizeof(rpc_addr_t));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(host);
	addr->sin_port = htons(port);

	rpcaddr->len = sizeof(struct sockaddr);

	return E_OK;
}

int rpc_addr_unpack(uint32_t *host, uint16_t *port, rpc_addr_t *rpc_addr)
{
	struct sockaddr_in *addr = (struct sockaddr_in *)&(rpc_addr->addr);

	*host = ntohl(addr->sin_addr.s_addr);
	*port = ntohs(addr->sin_port);

	return E_OK;
}

char *sock_htoa(uint32_t host)
{
	struct in_addr inaddr;

	inaddr.s_addr = htonl(host);
	return inet_ntoa(inaddr);
}

uint32_t sock_atoh(const char *host_name)
{
	return ntohl(inet_addr(host_name));
}

static void rpc_msg_serialize(uint8_t *buf, size_t *buf_len, rpc_msg_t *msg)
{
	*buf_len = 0;

	buf[0] = (msg->cmd << 1) | msg->rqrs;
	buf[1] = msg->data_len & 0xFF;
	buf[2] = (msg->data_len << 8) & 0xFF;
	*buf_len += 3;

	memcpy(&buf[3], msg->data, msg->data_len);
	*buf_len += msg->data_len;

	buf[*buf_len] = 0 - calculate_checksum(buf, *buf_len);
	*buf_len += 1;
}

static int rpc_msg_deserialize(rpc_msg_t *msg, uint8_t *buf, size_t buf_len)
{
	if (calculate_checksum(buf, buf_len) != 0)
		return E_CHECKSUM;

	msg->cmd = buf[0] >> 1;
	msg->rqrs = buf[0] & 0x01;
	msg->data_len = buf[2] << 8 | buf[1];

	memcpy(msg->data, &buf[3], msg->data_len);

	return E_OK;
}

static void print_msg(rpc_msg_t *msg)
{
	printf("> cmd     : 0x%02x\n", msg->cmd);
	printf("> rqrs    : 0x%02x\n", msg->rqrs);
	printf("> data len: %d\n", msg->data_len);
}
