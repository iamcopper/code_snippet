/**
 * \file
 *
 * The RPC interface define
 */

#ifndef __RPC_INTF_H__
#define __RPC_INTF_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include "rpc_msg.h"

/* Error Code */
#define E_OK                   0
#define E_PARAM                -1
#define E_SOCKET               -2
#define E_SEND                 -3
#define E_RECV                 -4
#define E_CHECKSUM             -5

typedef struct {
	struct sockaddr addr;
	socklen_t len;
} rpc_addr_t;

/* RPC server default host and port */
#define RPC_SERVER_HOST        "127.0.0.1"
#define RPC_SERVER_PORT        8088

/* RPC intf open flag */
#define OFLAG_CLIENT          0
#define OFLAG_SERVER          1

typedef struct rpc_intf_s {
	int fd;
	int oflag;
} rpc_intf_t;

extern rpc_intf_t *rpc_intf_open(int oflag, rpc_addr_t *servaddr);
extern int rpc_intf_close(rpc_intf_t *intf);

extern int rpc_intf_sendto(rpc_intf_t *intf, rpc_msg_t *msg, rpc_addr_t *dst);
extern int rpc_intf_recvfrom(rpc_intf_t *intf, rpc_msg_t *msg, rpc_addr_t *src);

extern int rpc_addr_pack(rpc_addr_t *rpcaddr, uint32_t host, uint16_t port);
extern int rpc_addr_unpack(uint32_t *host, uint16_t *port, rpc_addr_t *rpc_addr);

extern char *sock_htoa(uint32_t host);
extern uint32_t sock_atoh(const char *host_name);

#endif /* __RPC_INTF_H__ */
