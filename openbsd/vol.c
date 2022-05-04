#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sndio.h>

#define INTERVAL 0
#define FMT_SIZE 10

int volume = 0;
unsigned int max = 0;

void ondesc(void *arg, struct sioctl_desc *d, int curval)
{
	if (d == NULL || d->addr != 15)
		return;
	volume = curval;
	max = d->maxval;
}

void put(char *fmt)
{
	struct sioctl_hdl *hdl;

	hdl = sioctl_open(SIO_DEVANY, SIOCTL_READ, 0);
	if (hdl == NULL) {
		err(1, "sioctl_open");
	}

	if (!sioctl_ondesc(hdl, ondesc, NULL)) {
		err(1, "sioctl_ondesc");
	}

	printf(fmt, volume * 100.0 / max);

	sioctl_close(hdl);
}

int main(int argc, char *argv[])
{
	int opt;
	int itv = INTERVAL;
	char fmt[FMT_SIZE] = "%3.2f\n";
	struct sioctl_hdl *hdl;

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
