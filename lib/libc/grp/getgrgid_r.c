#include <grp.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int getgrgid_r(gid_t gid, struct group *grp, char *buf, size_t buflen,
	       struct group **result)
{
	FILE *f = fopen("/etc/group", "r");
	if (!f) {
		*result = NULL;
		return errno;
	}

	char line[LINE_MAX];
	char *members[1024];

	while (fgets(line, sizeof(line), f)) {
		char *name = strtok(line, ":");
		char *passwd = strtok(NULL, ":");
		char *gid_str = strtok(NULL, ":");
		char *mem_str = strtok(NULL, "\n");

		if (!name || !passwd || !gid_str)
			continue;

		gid_t file_gid = (gid_t)strtoul(gid_str, NULL, 10);
		if (file_gid != gid)
			continue;

		size_t len = strlen(name) + 1;
		if (len > buflen) {
			fclose(f);
			*result = NULL;
			return ERANGE;
		}
		memcpy(buf, name, len);
		grp->gr_name = buf;
		buf += len;
		buflen -= len;

		grp->gr_gid = file_gid;

		int i = 0;
		if (mem_str && *mem_str) {
			char *tok = strtok(mem_str, ",");
			while (tok && i < 1023) {
				size_t l = strlen(tok) + 1;
				if (l > buflen) {
					fclose(f);
					*result = NULL;
					return ERANGE;
				}
				memcpy(buf, tok, l);
				members[i++] = buf;
				buf += l;
				buflen -= l;
				tok = strtok(NULL, ",");
			}
		}
		members[i] = NULL;
		grp->gr_mem = members;

		fclose(f);
		*result = grp;
		return 0;
	}

	fclose(f);
	*result = NULL;
	return 0;
}
