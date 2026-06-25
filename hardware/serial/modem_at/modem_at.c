/*
 * modem_at.c - Send AT commands over serial port, print response.
 * Usage: modem_at --port /dev/ttyUSB2 --cmd "AT+CPIN?" [--timeout 3]
 *
 * Compile: gcc -o modem_at modem_at.c
 * Requires: POSIX termios, select(), getopt_long (glibc)
 */

#define _GNU_SOURCE   // for getopt_long
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <getopt.h>

#define BUF_SIZE 4096

static int set_serial_attrs(int fd, speed_t baud) {
	struct termios tty;
	memset(&tty, 0, sizeof(tty));
	if (tcgetattr(fd, &tty) != 0) {
		perror("tcgetattr");
		return -1;
	}
	cfsetospeed(&tty, baud);
	cfsetispeed(&tty, baud);

	tty.c_cflag &= ~PARENB;   // no parity
	tty.c_cflag &= ~CSTOPB;   // 1 stop bit
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;       // 8 bits
	tty.c_cflag |= CLOCAL;    // ignore modem control lines
	tty.c_cflag |= CREAD;     // enable receiver

	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
	tty.c_oflag &= ~OPOST;                           // raw output
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // no flow control
	tty.c_iflag &= ~(INLCR | ICRNL | IGNCR);        // no CR/LF conversion

	tty.c_cc[VMIN]  = 0;   // non-blocking read
	tty.c_cc[VTIME] = 1;   // inter-character timeout (0.1 sec)

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		perror("tcsetattr");
		return -1;
	}
	return 0;
}

static void trim_trailing_newlines(char *s) {
	size_t len = strlen(s);
	while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r'))
		s[--len] = '\0';
}

int main(int argc, char *argv[]) {
	const char *port = NULL;
	const char *cmd = NULL;
	int timeout_sec = 3;

	// Define long options
	static struct option long_options[] = {
		{"port",    required_argument, NULL, 'p'},
		{"cmd",     required_argument, NULL, 'c'},
		{"timeout", required_argument, NULL, 't'},
		{0, 0, 0, 0}
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "p:c:t:", long_options, NULL)) != -1) {
		switch (opt) {
		case 'p':
			port = optarg;
			break;
		case 'c':
			cmd = optarg;
			break;
		case 't':
			timeout_sec = atoi(optarg);
			if (timeout_sec <= 0) timeout_sec = 3;
			break;
		default:
			fprintf(stderr, "Usage: %s --port <port> --cmd <command> [--timeout <seconds>]\n", argv[0]);
			return 1;
		}
	}

	if (!port || !cmd) {
		fprintf(stderr, "Usage: %s --port <port> --cmd <command> [--timeout <seconds>]\n", argv[0]);
		return 1;
	}

	// Open serial port
	int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("__SERIAL_ERROR: cannot open %s: %s\n", port, strerror(errno));
		return 1;
	}

	if (set_serial_attrs(fd, B9600) != 0) {
		close(fd);
		printf("__ERROR: failed to configure serial port\n");
		return 1;
	}

	// Flush input buffer
	tcflush(fd, TCIFLUSH);

	// Build command string with \r
	size_t cmd_len = strlen(cmd);
	char *full_cmd = malloc(cmd_len + 2);
	if (!full_cmd) {
		printf("__ERROR: memory allocation failed\n");
		close(fd);
		return 1;
	}
	memcpy(full_cmd, cmd, cmd_len);
	full_cmd[cmd_len] = '\r';
	full_cmd[cmd_len+1] = '\0';

	// Write command
	ssize_t written = write(fd, full_cmd, cmd_len+1);
	free(full_cmd);
	if (written != (ssize_t)(cmd_len+1)) {
		printf("__SERIAL_ERROR: write failed: %s\n", strerror(errno));
		close(fd);
		return 1;
	}

	// Read response with timeout
	char buf[BUF_SIZE];
	size_t total = 0;
	struct timeval start, now;
	gettimeofday(&start, NULL);

	while (1) {
		// Check timeout
		gettimeofday(&now, NULL);
		double elapsed = (now.tv_sec - start.tv_sec) +
			(now.tv_usec - start.tv_usec) / 1e6;
		if (elapsed >= timeout_sec) {
			break; // timeout
		}

		// Use select with remaining time
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		struct timeval tv;
		double remain = timeout_sec - elapsed;
		tv.tv_sec = (long)remain;
		tv.tv_usec = (long)((remain - tv.tv_sec) * 1e6);
		if (tv.tv_sec < 0) tv.tv_sec = 0;
		if (tv.tv_usec < 0) tv.tv_usec = 0;

		int ret = select(fd+1, &rfds, NULL, NULL, &tv);
		if (ret < 0) {
			if (errno == EINTR) continue;
			printf("__SERIAL_ERROR: select failed: %s\n", strerror(errno));
			break;
		}
		if (ret == 0) {
			break; // timeout
		}

		// Data available
		char tmp[256];
		ssize_t n = read(fd, tmp, sizeof(tmp)-1);
		if (n < 0) {
			printf("__SERIAL_ERROR: read failed: %s\n", strerror(errno));
			break;
		}
		if (n == 0) {
			break; // EOF (should not happen)
		}
		tmp[n] = '\0';

		// Append to buffer
		if (total + n < BUF_SIZE - 1) {
			memcpy(buf + total, tmp, n);
			total += n;
			buf[total] = '\0';
		} else {
			// Buffer full, stop reading but still check for OK/ERROR
			break;
		}

		// Check if we have seen OK or ERROR
		if (strstr(buf, "OK\r\n") != NULL || strstr(buf, "ERROR\r\n") != NULL) {
			break;
		}
	}

	close(fd);

	// Trim trailing newlines and print
	trim_trailing_newlines(buf);
	printf("%s\n", buf);

	return 0;
}
