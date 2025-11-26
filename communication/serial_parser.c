/**
 * 串口数据解析程序
 * 1. 程序分1次或多次接收从串口发来的数据，根据需要完成数据组包，并解析，根据其中的命令字段做不同的响应。
 * 2. 完整的数据包格式如下：包含包头1字节，命令1字节，数据长度2字节，实际数据(长度由数据长度字段指定)，校验码1字节。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>

// 数据包结构定义
#pragma pack(1)
typedef struct {
	uint8_t header;     // 包头 (0xFF)
	uint8_t command;    // 命令
	uint16_t length;    // 数据长度
	uint8_t data[0];    // 可变长度数据
} serial_packet_t;
#pragma pack()

// 校验和计算
uint8_t calculate_checksum(const uint8_t *data, uint16_t length) {
	uint8_t checksum = 0;
	for (uint16_t i = 0; i < length; i++) {
		checksum ^= data[i];
	}
	return checksum;
}

// 串口初始化
int serial_init(const char *device, int baudrate) {
	int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		perror("无法打开串口");
		return -1;
	}

	struct termios options;
	tcgetattr(fd, &options);

	// 设置波特率
	cfsetispeed(&options, baudrate);
	cfsetospeed(&options, baudrate);

	// 设置数据位：8位
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	// 设置奇偶校验：无
	options.c_cflag &= ~PARENB;
	options.c_iflag &= ~INPCK;

	// 设置停止位：1位
	options.c_cflag &= ~CSTOPB;

	// 关闭硬件流控
	options.c_cflag &= ~CRTSCTS;

	// 启用接收
	options.c_cflag |= (CLOCAL | CREAD);

	// 关闭软件流控
	options.c_iflag &= ~(IXON | IXOFF | IXANY);

	// 原始模式输入
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	// 设置超时
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10; // 1秒超时

	tcsetattr(fd, TCSANOW, &options);
	return fd;
}

// 数据包解析器类
typedef struct {
	uint8_t *buffer;        // 接收缓冲区
	uint16_t buffer_size;   // 缓冲区大小
	uint16_t data_len;      // 当前数据长度
	uint8_t packet_header;  // 包头标识
} packet_parser_t;

// 初始化解析器
packet_parser_t* parser_init(uint16_t buffer_size, uint8_t header) {
	packet_parser_t *parser = malloc(sizeof(packet_parser_t));
	parser->buffer = malloc(buffer_size);
	parser->buffer_size = buffer_size;
	parser->data_len = 0;
	parser->packet_header = header;
	return parser;
}

// 释放解析器
void parser_free(packet_parser_t *parser) {
	if (parser) {
		free(parser->buffer);
		free(parser);
	}
}

// 处理不同命令的函数
void handle_command_0x01(const uint8_t *data, uint16_t length) {
	printf("处理命令 0x01 - 数据长度: %d\n", length);
	printf("数据内容: ");
	for (int i = 0; i < length && i < 16; i++) {
		printf("%02X ", data[i]);
	}
	printf("\n");
}

void handle_command_0x02(const uint8_t *data, uint16_t length) {
	printf("处理命令 0x02 - 数据长度: %d\n", length);
	if (length >= 4) {
		uint32_t value = *(uint32_t*)data;
		printf("数值: %u\n", value);
	}
}

void handle_command_0x03(const uint8_t *data, uint16_t length) {
	printf("处理命令 0x03 - 字符串数据\n");
	if (length > 0) {
		printf("内容: ");
		for (int i = 0; i < length; i++) {
			if (data[i] >= 32 && data[i] <= 126) {
				printf("%c", data[i]);
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
}

// 默认命令处理
void handle_unknown_command(uint8_t command, const uint8_t *data, uint16_t length) {
	printf("未知命令 0x%02X - 数据长度: %d\n", command, length);
}

// 解析数据包
int parse_packet(packet_parser_t *parser, const uint8_t *data, uint16_t length) {
	int packets_processed = 0;

	// 将新数据添加到缓冲区
	if (parser->data_len + length > parser->buffer_size) {
		// 缓冲区溢出，清空缓冲区重新开始
		parser->data_len = 0;
		return 0;
	}

	memcpy(parser->buffer + parser->data_len, data, length);
	parser->data_len += length;

	// 处理缓冲区中的所有完整数据包
	while (parser->data_len >= 5) { // 最小包长度：包头1+命令1+长度2+校验1=5字节
									// 查找包头
		int packet_start = -1;
		for (int i = 0; i <= parser->data_len - 5; i++) {
			if (parser->buffer[i] == parser->packet_header) {
				packet_start = i;
				break;
			}
		}

		if (packet_start == -1) {
			// 没有找到包头，清空无效数据
			parser->data_len = 0;
			break;
		}

		if (packet_start > 0) {
			// 移除包头前的无效数据
			memmove(parser->buffer, parser->buffer + packet_start, parser->data_len - packet_start);
			parser->data_len -= packet_start;
		}

		if (parser->data_len < 5) {
			break; // 数据不够，等待更多数据
		}

		// 获取数据长度
		uint16_t data_length = *(uint16_t*)(parser->buffer + 2);

		// 检查是否收到完整数据包
		uint16_t total_packet_length = 5 + data_length; // 包头1+命令1+长度2+数据N+校验1
		if (parser->data_len < total_packet_length) {
			break; // 数据不完整，等待更多数据
		}

		// 提取完整数据包
		serial_packet_t *packet = (serial_packet_t*)parser->buffer;

		// 验证校验和
		uint8_t expected_checksum = calculate_checksum(parser->buffer + 1, 3 + data_length); // 命令+长度+数据
		uint8_t actual_checksum = parser->buffer[4 + data_length];

		if (expected_checksum != actual_checksum) {
			printf("校验和错误: 期望 0x%02X, 实际 0x%02X\n", expected_checksum, actual_checksum);
			// 移除包头，继续处理后续数据
			memmove(parser->buffer, parser->buffer + 1, parser->data_len - 1);
			parser->data_len--;
			continue;
		}

		// 处理有效数据包
		switch (packet->command) {
		case 0x01:
			handle_command_0x01(packet->data, data_length);
			break;
		case 0x02:
			handle_command_0x02(packet->data, data_length);
			break;
		case 0x03:
			handle_command_0x03(packet->data, data_length);
			break;
		default:
			handle_unknown_command(packet->command, packet->data, data_length);
			break;
		}

		packets_processed++;

		// 移除已处理的数据包
		memmove(parser->buffer, parser->buffer + total_packet_length, parser->data_len - total_packet_length);
		parser->data_len -= total_packet_length;
	}

	return packets_processed;
}

// 主函数
int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("用法: %s <串口设备> <波特率>\n", argv[0]);
		printf("示例: %s /dev/ttyUSB0 115200\n", argv[0]);
		return 1;
	}

	const char *device = argv[1];
	int baudrate = atoi(argv[2]);

	// 初始化串口
	int fd = serial_init(device, baudrate);
	if (fd == -1) {
		return 1;
	}

	printf("串口 %s 打开成功，波特率: %d\n", device, baudrate);

	// 初始化数据包解析器
	packet_parser_t *parser = parser_init(4096, 0xFF); // 4KB缓冲区，包头0xFF

	uint8_t read_buffer[256];

	while (1) {
		// 读取串口数据
		int bytes_read = read(fd, read_buffer, sizeof(read_buffer));
		if (bytes_read > 0) {
			printf("收到 %d 字节数据\n", bytes_read);

			// 解析数据包
			int packets = parse_packet(parser, read_buffer, bytes_read);
			if (packets > 0) {
				printf("成功解析 %d 个数据包\n", packets);
			}
		} else if (bytes_read == 0) {
			// 无数据，短暂等待
			usleep(10000); // 10ms
		} else {
			if (errno != EAGAIN) {
				perror("读取串口错误");
				break;
			}
		}
	}

	// 清理资源
	close(fd);
	parser_free(parser);

	return 0;
}
