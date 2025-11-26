/**
 * \file
 *
 * The RPC message define
 */

#ifndef __RPC_MSG_H__
#define __RPC_MSG_H__

#include <stdint.h>

#define RPC_BUFFER_SIZE           512

/*
 * RPC message CMD list
 */
#define CMD_LOGIN                 0x00
#define CMD_EXIT                  0x01
#define CMD_GET_SERVER_NUM        0x02
#define CMD_GET_SERVER_INFO       0x03
#define CMD_ATTACH_SERVER         0x04
#define CMD_DETACH_SERVER         0x05
#define CMD_SWITCH_RIGHTS         0x06
#define CMD_SOL_DATA              0x0F

#define RQRS_REQUEST              0
#define RQRS_RESPONSE             1

/* Complete Code */
#define CC_OK                     0x00
#define CC_INVALID_SESSION_NO     0x01

typedef struct {
	uint8_t cmd:7;
	uint8_t rqrs:1;
	uint16_t data_len;
	uint8_t *data;
} rpc_msg_t;

/* Server state */
#define SERVER_ST_OK                 0x00
#define SERVER_ST_NOT_SUPPORT_SOL    0x01
#define SERVER_ST_RMCPP_SESSION_ERR  0x02

/* Server info format */
struct {
	uint8_t ipmb_addr;
	uint32_t hostname;
	uint16_t port;
	char desc[16];
	uint8_t state;
} server_info;

#endif /* __RPC_MSG_H__ */
