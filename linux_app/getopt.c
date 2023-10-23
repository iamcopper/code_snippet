#include <stdio.h>
#include <unistd.h>

/*
 * man 3 getopt/optarg/optind/opterr/optopt
 * int getopt(int argc, char * const argv[], const char *optstring);
 * extern char *optarg;
 * extern int optind, opterr, optopt;
 */

#define OPTSTRING "g:s:h"

void usage(char *appname)
{
	printf("%s <opts>\n\n", appname);
	printf("opts can be:\n");
	printf("  -g <arg3>  xxxxx\n");
	printf("  -s <arg4>  xxxxx\n");
	printf("  -h         print this usage\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	int argflag = 0;

	while ((argflag = getopt(argc, (char **)argv, OPTSTRING)) != -1) {

		/* argv[optind] -> next arg */
		printf("argv[optind]=%s\n", argv[optind]);

		switch (argflag) {
		case 'g':
			printf("-g optarg=%s\n", optarg);
			break;
		case 's':
			printf("-s optarg=%s\n", optarg);
			break;
		case 'h':
		default:
			usage(argv[0]);
			return 0;
			break;
		}
	}

	return 0;
}
