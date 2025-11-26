#include <stddef.h>
#include "rpc_cmd.h"

#include <string.h>
#include <stdio.h>

static void rpc_cmd_login(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_exit(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_get_server_num(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_get_server_info(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_attach_server(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_detach_server(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_switch_rights(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

static void rpc_cmd_sol_data(rpc_msg_t *rq, rpc_msg_t *rs)
{
	printf("%s\n", __func__);
}

typedef struct {
	void (*func)(rpc_msg_t *rq, rpc_msg_t *rs);
	uint8_t cmd;
} rpc_cmd_list_t;

rpc_cmd_list_t rpc_cmd_list[] = {
	{ rpc_cmd_login,            CMD_LOGIN              },
	{ rpc_cmd_exit,             CMD_EXIT               },
	{ rpc_cmd_get_server_num,   CMD_GET_SERVER_NUM     },
	{ rpc_cmd_get_server_info,  CMD_GET_SERVER_INFO    },
	{ rpc_cmd_attach_server,    CMD_ATTACH_SERVER      },
	{ rpc_cmd_detach_server,    CMD_DETACH_SERVER      },
	{ rpc_cmd_switch_rights,    CMD_SWITCH_RIGHTS      },
	{ rpc_cmd_sol_data,         CMD_SOL_DATA           },
	{ NULL },
};

int rpc_cmd_run(rpc_msg_t *rq, rpc_msg_t *rs)
{
	rpc_cmd_list_t *rpc_cmd = rpc_cmd_list;

	for (rpc_cmd = rpc_cmd_list; rpc_cmd->func != NULL; rpc_cmd++)
	{
		if (rpc_cmd->cmd == rq->cmd)
			break;
	}

	memcpy(rs, rq, sizeof(rpc_msg_t));
	if (rpc_cmd->func != NULL)
	{
		rpc_cmd->func(rq, rs);
		return 0;
	}

	return -1;
}
