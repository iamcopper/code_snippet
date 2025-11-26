/*-
 * Copyright (c) 2019 Advantech
 * All rights reserved.
 *
 * $Id: solproxy.c 282 2019-01-04 01:47:08Z kang.pan@advantech.com.cn $
 */

/**
 * \file
 * solproxy
 */

#include <stdio.h>
#include "rpc_intf.h"
#include "rpc_cmd.h"

int main(int argc, char *argv[])
{
	rpc_intf_t *rpc_intf = NULL;
	rpc_addr_t servaddr, clientaddr;
	rpc_msg_t rq, rs;
	uint32_t host;
	uint16_t port;

	rpc_addr_pack(&servaddr, INADDR_ANY, RPC_SERVER_PORT);
	rpc_intf = rpc_intf_open(OFLAG_SERVER, &servaddr);

	while (1)
	{
		rpc_intf_recvfrom(rpc_intf, &rq, &clientaddr);

		rpc_addr_unpack(&host, &port, &clientaddr);
		printf("Receive message from %s:%d\n", sock_htoa(host), htons(port));

		rpc_cmd_run(&rq, &rs);
		rs.rqrs = RQRS_RESPONSE;
		rpc_intf_sendto(rpc_intf, &rs, &clientaddr);
	}

	rpc_intf_close(rpc_intf);

	return 0;
}
