#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#define TIME_STR_FOMAT   "%Y%m%d %H:%M:%S"
#define TIME_STR_SIZE    15

void mylog(FILE *stream, const char *fmt, ...)
{
	va_list args;

	time_t rawtime = time(NULL);
	struct tm *timeinfo = localtime(&rawtime);
	char tmstr[TIME_STR_SIZE] = {0};
	
	strftime(tmstr, TIME_STR_SIZE, TIME_STR_FOMAT, timeinfo);

	fprintf(stream, "[%s] ", tmstr);

	va_start(args, fmt);
	vfprintf(stream, fmt, args);
	va_end(args);

	fprintf(stream, "\n");
}

int main(int argc, char *argv[])
{
	mylog(stderr, "Hello World.");
	return 0;
}
