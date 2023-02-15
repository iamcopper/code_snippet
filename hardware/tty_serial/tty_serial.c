#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

int tty_serial_init(int fd)
{
	int ret = 0;
	struct termios options;

	ret = tcgetattr(fd, &options);
	if (ret < 0) {
		perror("tcgetattr");
		return -1;
	}

	/* Baudrate: 115200 */
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	/* Data bits: 8 */
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	/* STOP bits: 1 */
	options.c_cflag &= ~CSTOPB;

	/* Parity: 0 */
	options.c_cflag &= ~PARENB;

	/* Disable RTS/CTS hardware flow control */
	options.c_cflag &= ~CRTSCTS;

	/* Raw Mode */
	/* man cfmakeraw --- cfmakeraw(&options); */
	options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
							| INLCR | IGNCR | ICRNL | IXON);
	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

	ret = tcsetattr(fd, TCSANOW, &options);
	if (ret < 0) {
		perror("tcsetattr");
		return -1;
	}

	return 0;
}
