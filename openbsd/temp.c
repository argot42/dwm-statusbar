#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/sensors.h>

#define INTERVAL 3
#define FMT_SIZE 10
#define MIB_SIZE 5

void put(char *fmt)
{
	int mib[MIB_SIZE];
	struct sensor s;
	size_t len;

	mib[0] = CTL_HW;
	mib[1] = HW_SENSORS;
	mib[2] = 0;
	mib[3] = SENSOR_TEMP;
	mib[4] = 0;

	len = sizeof(s);
	if (sysctl(mib, MIB_SIZE, &s, &len, NULL, 0) == -1) {
		err(1, "sysctl");
	}

	if (printf(fmt, (s.value - 273150000) / 1000000.0) == -1) {
		err(1, "printf");
	}
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	int opt;
	int itv = INTERVAL;
	char fmt[FMT_SIZE] = "%3.1f\n";

	while((opt = getopt(argc, argv, "hf:i:")) != -1) {
		switch(opt) {
		case 'h':
			fprintf(stderr, "%s [-h|-f fmt|-i interval]\n", argv[0]);
			exit(EXIT_FAILURE);
		case 'i':
			itv = atoi(optarg);
			break;
		case 'f':
			snprintf(fmt, FMT_SIZE, "%s\n", optarg);
			break;
		}
	}

	if (itv <= 0) {
		put(fmt);
		exit(EXIT_SUCCESS);
	}

	while(1) {
		put(fmt);
		sleep(itv);
	}
	exit(EXIT_SUCCESS);
}	
