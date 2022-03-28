#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
int main(int argc,char *argv[])
{
	printf("now time: %lu\n", time(NULL));

	struct timeval tmv = {0};
	gettimeofday(&tmv, NULL);
	printf("gettimeofday: (%lu, %lu)\n", tmv.tv_sec, tmv.tv_usec);

	struct timespec tms = {0};
	clock_gettime(CLOCK_REALTIME, &tms);
	printf("CLOCK_REALTIME: (%lu, %lu)\n", tms.tv_sec, tms.tv_nsec);
	clock_gettime(CLOCK_MONOTONIC, &tms);
	printf("CLOCK_MONOTONIC: (%lu, %lu)\n", tms.tv_sec, tms.tv_nsec);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tms);
	printf("CLOCK_PROCESS_CPUTIME_ID: (%lu, %lu)\n", tms.tv_sec, tms.tv_nsec);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tms);
	printf("CLOCK_THREAD_CPUTIME_ID: (%lu, %lu)\n", tms.tv_sec, tms.tv_nsec);

	return 0;
}
