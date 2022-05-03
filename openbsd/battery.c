#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/sensors.h>

#define INTERVAL 0
#define FMT_SIZE 100
#define MIB_SIZE 5

void put(char *fmt)
{
	int mib[MIB_SIZE];
	struct sensor s;
	size_t len;
	float total;
	float current;
	int64_t charging;

	mib[0] = CTL_HW;
	mib[1] = HW_SENSORS;
	mib[2] = 3;	
	mib[3] = SENSOR_WATTHOUR;
	mib[4] = 3;

	len = sizeof(s);
	if (sysctl(mib, MIB_SIZE, &s, &len, NULL, 0) == -1) {
		err(1, "sysctl: current");
	}
	current = s.value / 1000000.0;

	mib[4] = 4;
	if (sysctl(mib, MIB_SIZE, &s, &len, NULL, 0) == -1) {
		err(1, "sysctl: total");
	}
	total = s.value / 1000000.0;

	mib[3] = SENSOR_INTEGER;
	mib[4] = 0;
	if (sysctl(mib, MIB_SIZE, &s, &len, NULL, 0) == -1) {
		err(1, "sysctl: charging");
	}
	charging = s.value;

	if (printf(fmt, (int)charging, current * 100.0 / total) < 0) {
		err(1, "printf");	
	}
	if (fflush(stdout) != 0) {
		err(1, "fflush");
	}
}

int main(int argc, char *argv[])
{
	int opt;
	int itv = INTERVAL;
	char fmt[FMT_SIZE] = "%3.1f %d\n";

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
