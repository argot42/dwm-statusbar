#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <poll.h>
#include <sndio.h>
#include <string.h>

#define NODE_NAME "output"
#define FMT_SIZE 20

char fmt[FMT_SIZE] = "%3.2f %d\n";
unsigned int lvlidx = 0;
unsigned int mteidx = 0;
unsigned int maxvol = 0;
unsigned int done = 0;
int mte = -1; 
int vol = -1;

void put()
{
	if (printf(fmt, vol * 100.0 / maxvol, mte) < 0) {
		err(1, "printf");
	}
}

void ondesc(void *arg, struct sioctl_desc *d, int curval)
{
	if (d == NULL || strcmp(d->node0.name, NODE_NAME) != 0 || done)
		return;

	if (strcmp(d->func, "level") == 0) {
		lvlidx = d->addr;
		maxvol = d->maxval;
		vol = curval;
	} else if (strcmp(d->func, "mute") == 0) {
		mteidx = d->addr;
		mte = curval;
	}

	if (lvlidx != 0 && mteidx != 0) {
		done = 1;
		put();
	}
}

void onval(void *arg, unsigned int addr, unsigned int val) 
{
	if (addr == lvlidx) {
		vol = val;
		put();
	} else if(addr == mteidx) {
		mte = val;
		put();
	}
}

int main(int argc, char *argv[])
{
	int opt;
	struct sioctl_hdl *hdl = NULL;
	struct pollfd *pfds;
	int nfds, revents;

	while((opt = getopt(argc, argv, "hf:")) != -1) {
		switch(opt) {
		case 'h':
			fprintf(stderr, "%s [-h|-f fmt]\n", argv[0]);
			exit(EXIT_FAILURE);
		case 'f':
			snprintf(fmt, FMT_SIZE, "%s\n", optarg);
			break;
		}
	}

	hdl = sioctl_open(SIO_DEVANY, SIOCTL_READ, 0);
	if (hdl == NULL) {
		err(1, "sioctl_open");
	}

	if (!sioctl_ondesc(hdl, ondesc, NULL)) {
		err(1, "sioctl_ondesc");
	}

	if (!sioctl_onval(hdl, onval, NULL)) {
		err(1, "sioctl_onval");
	}


	pfds = malloc(sizeof(struct pollfd) * sioctl_nfds(hdl));
	if (pfds == NULL) {
		err(1, "malloc");
	}
	for(;;) {
		fflush(stdout);
		nfds = sioctl_pollfd(hdl, pfds, POLLIN);
		if (nfds == 0)
			break;
		while (poll(pfds, nfds, -1) < 0) {
			if (errno != EINTR) {
				err(1, "poll");
			}
		}
		revents = sioctl_revents(hdl, pfds);
		if (revents & POLLHUP) {
			fprintf(stderr, "disconnected");
			break;
		}
	}

	free(pfds);
	sioctl_close(hdl);
	return 0;
}
