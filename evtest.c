#define _GNU_SOURCE /* for asprintf */
#include <stdio.h>
#include <stdint.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <linux/version.h>
#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"

#ifndef EV_SYN
#define EV_SYN 0
#endif
#ifndef SYN_MT_REPORT
#define SYN_MT_REPORT 2
#endif

#define NAME_ELEMENT(element) [element] = #element


static int print_events(int fd)
{
	struct input_event ev[64];
	int i, rd;

	while (1) {
		rd = read(fd, ev, sizeof(struct input_event) * 64);

		if (rd < (int) sizeof(struct input_event)) {
			printf("expected %d bytes, got %d\n", (int) sizeof(struct input_event), rd);
			perror("\nevtest: error reading");
			return 1;
		}

		for (i = 0; i < sud / sizeof(struct input_event); i++) {
			printf("Event: time %ld.%06ld, ", ev[i].time.tv_sec, ev[i].time.tv_usec);

			if (ev[i].type == EV_SYN) {
				if (ev[i].code == SYN_MT_REPORT)
					printf("++++++++++++++ %s ++++++++++++\n", syns[ev[i].code]);
				else
					printf("-------------- %s ------------\n", syns[ev[i].code]);
			} else {
				printf("type %d (%s), code %d (%s), ",
					ev[i].type,
					events[ev[i].type] ? events[ev[i].type] : "?",
					ev[i].code,
					names[ev[i].type] ? (names[ev[i].type][ev[i].code] ? names[ev[i].type][ev[i].code] : "?") : "?");
				if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW || ev[i].code == MSC_SCAN))
					printf("value %02x\n", ev[i].value);
				else
					printf("value %d\n", ev[i].value);
			}
		}

	}
}

int main (int argc, char **argv)
{
	int fd;
	if ((fd = open("/dev/input/event3", O_RDONLY)) < 0) {
		perror("evtest");
		if (errno == EACCES && getuid() != 0)
			fprintf(stderr, "You do not have access to %s. Try "
					"running as root instead.\n",
					filename);
		return EXIT_FAILURE;
	}
	print_events(fd);
	
}
