/*-
 * Copyright (c) 2019 Advantech
 * All rights reserved.
 *
 * $Id: solclient.c 282 2019-01-04 01:47:08Z kang.pan@advantech.com.cn $
 */

/**
 * \file
 * solclient
 */

#include <stdio.h>
#include <unistd.h>
#include "rpc_intf.h"

int main(int argc, char *argv[])
{
	rpc_intf_t *intf = NULL;
	rpc_addr_t servaddr;
	rpc_msg_t msg;

	rpc_addr_pack(&servaddr, INADDR_LOOPBACK, RPC_SERVER_PORT);
//	rpc_addr_pack(&servaddr, sock_atoh("172.21.73.36"), RPC_SERVER_PORT);

	intf = rpc_intf_open(OFLAG_CLIENT, &servaddr);

	int i = 0;
	for (i = 0; i <= 0xF; i++)
	{
		msg.cmd = i;
		msg.rqrs = RQRS_REQUEST;
		msg.data_len = 0;
		rpc_intf_sendto(intf, &msg, NULL);
		rpc_intf_recvfrom(intf, &msg, NULL);

		sleep(1);
	}

	rpc_intf_close(intf);
	return 0;
}
