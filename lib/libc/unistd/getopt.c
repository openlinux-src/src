#include <unistd.h>
#include <stdio.h>

char *optarg;
int optind = 1, opterr = 1, optopt;

int getopt(int argc, char *const argv[], const char *optstring)
{
	char *optchar;
	char c, d;
	int i;
	static int optpos;

	if (optind == 0) {
		optpos = 0;
		optind = 1;
	}

	if (optind >= argc || !argv[optind]) {
		return -1;
	}

	if (argv[optind][0] != '-') {
		if (optstring[0] == '-') {
			optarg = argv[optind++];
			return 1;
		}

		return -1;
	}

	if (argv[optind][1] == 0) {
		return -1;
	}

	if (argv[optind][1] == '-' && argv[optind][2] == 0) {
		return optind++, -1;
	}

	if (optpos == 0)
		optpos++;

	c = argv[optind][optpos];
	optchar = argv[optind] + optpos++;

	if (argv[optind][optpos] == 0) {
		optind++;
		optpos = 0;
	}

	i = 0;
	d = 0;

	do {
		d = optstring[i++];
	} while (d && d != c);

	if (d != c || c == ':') {
		optopt = c;
		if (optstring[0] != ':' && opterr) {
			fprintf(stderr, "%s: invalid option -- %c\n", argv[0],
				*optchar);
		}
		return '?';
	}

	if (optstring[i] == ':') {
		optarg = 0;
		if (optstring[i + 1] != ':' || optpos) {
			optarg = argv[optind++];
			if (optpos) {
				optarg += optpos;
			}
			optpos = 0;
		}
		if (optind > argc) {
			optopt = c;
			if (optstring[0] == ':') {
				return ':';
			}
			if (opterr) {
				fprintf(stderr,
					"%s: option requires an argument: %c\n",
					argv[0], *optchar);
			}
			return '?';
		}
	}

	return c;
}
