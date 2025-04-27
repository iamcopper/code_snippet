#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

//#define __DEBUG__
//#define __DAEMON__

#ifdef __DEBUG__
#define dprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define dperror(str)      perror(str)
#define dlog(fmt, ...)    fprintf(stderr, "[%s:%d] "fmt, __func__, __LINE__, ##__VA_ARGS__)
#elif defined __DAEMON__
#define dprintf(fmt, ...) syslog(LOG_INFO, fmt, ##__VA_ARGS__)
#define dperror(str)      syslog(LOG_ERR, str" : %s", strerror(errno))
#define dlog(fmt, ...)    syslog(LOG_INFO, "[%s:%d] "fmt, __func__, __LINE__, ##__VA_ARGS__)
#else
#define dprintf(fmt, ...)
#define dperror(str)
#define dlog(fmt, ...)
#endif

int main(int argc, char *argv[])
{
#ifdef __DAEMON__
	openlog(argv[0], LOG_CONS | LOG_PID, LOG_USER);
#endif

	dprintf("%s %d\n", "hello", 1);
	dperror("hello 2");
	dlog("%s %d\n", "hello", 3);

#ifdef __DAEMON__
	closelog();
#endif
	return 0;
}
