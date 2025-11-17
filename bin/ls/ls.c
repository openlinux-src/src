#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>

#define major(x)                                      \
	((unsigned)((((x) >> 31 >> 1) & 0xfffff000) | \
		    (((x) >> 8) & 0x00000fff)))
#define minor(x) ((unsigned)((((x) >> 12) & 0xffffff00) | ((x) & 0x000000ff)))

#define FLAG_LONG	(1 << 0)
#define FLAG_ALL	(1 << 1)
#define FLAG_ALMOST_ALL (1 << 2)
#define FLAG_CLASSIFY	(1 << 3)
#define FLAG_INODE	(1 << 4)
#define FLAG_SIZE	(1 << 5)
#define FLAG_REVERSE	(1 << 6)
#define FLAG_TIME_SORT	(1 << 7)
#define FLAG_SIZE_SORT	(1 << 8)
#define FLAG_RECURSIVE	(1 << 9)
#define FLAG_DIRECTORY	(1 << 10)
#define FLAG_ONE_LINE	(1 << 11)
#define FLAG_COLUMNS	(1 << 12)
#define FLAG_QUOTE	(1 << 13)
#define FLAG_KILO	(1 << 14)
#define FLAG_NUMERIC	(1 << 15)
#define FLAG_NO_GROUP	(1 << 16)
#define FLAG_SLASH_DIR	(1 << 17)
#define FLAG_FOLLOW_ARG (1 << 18)
#define FLAG_FOLLOW_ALL (1 << 19)
#define FLAG_COMMA	(1 << 20)
#define FLAG_ACROSS	(1 << 21)
#define FLAG_CTIME	(1 << 22)
#define FLAG_ATIME	(1 << 23)
#define FLAG_NO_SORT	(1 << 24)

struct entry {
	char *name;
	char *path;
	struct stat st;
	int is_link;
	char link_target[PATH_MAX];
};

static int flags = 0;

static void usage(void)
{
	fprintf(stderr,
		"usage: ls [-ikqrs] [-glno] [-A|-a] [-C|-m|-x|-1] "
		"[-F|-p] [-H|-L] [-R|-d] [-S|-f|-t] [-c|-u] [file...]\n");
}

static int should_show_file(const char *name)
{
	if (flags & FLAG_ALL)
		return 1;

	if (flags & FLAG_ALMOST_ALL) {
		if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
			return 0;
		return 1;
	}

	return name[0] != '.';
}

static char get_file_type_char(mode_t mode)
{
	if (S_ISDIR(mode))
		return 'd';
	if (S_ISLNK(mode))
		return 'l';
	if (S_ISBLK(mode))
		return 'b';
	if (S_ISCHR(mode))
		return 'c';
	if (S_ISFIFO(mode))
		return 'p';
	if (S_ISSOCK(mode))
		return 's';
	return '-';
}

static void format_permissions(mode_t mode, char *str)
{
	str[0] = get_file_type_char(mode);

	str[1] = (mode & S_IRUSR) ? 'r' : '-';
	str[2] = (mode & S_IWUSR) ? 'w' : '-';
	if (mode & S_ISUID) {
		str[3] = (mode & S_IXUSR) ? 's' : 'S';
	} else {
		str[3] = (mode & S_IXUSR) ? 'x' : '-';
	}

	str[4] = (mode & S_IRGRP) ? 'r' : '-';
	str[5] = (mode & S_IWGRP) ? 'w' : '-';
	if (mode & S_ISGID) {
		str[6] = (mode & S_IXGRP) ? 's' : 'S';
	} else {
		str[6] = (mode & S_IXGRP) ? 'x' : '-';
	}

	str[7] = (mode & S_IROTH) ? 'r' : '-';
	str[8] = (mode & S_IWOTH) ? 'w' : '-';
	if (mode & S_ISVTX) {
		str[9] = (mode & S_IXOTH) ? 't' : 'T';
	} else {
		str[9] = (mode & S_IXOTH) ? 'x' : '-';
	}

	str[10] = '\0';
}

static char get_classify_char(mode_t mode)
{
	if (S_ISDIR(mode))
		return '/';
	if (S_ISLNK(mode))
		return '@';
	if (S_ISFIFO(mode))
		return '|';
	if (S_ISSOCK(mode))
		return '=';
	if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
		return '*';
	return '\0';
}

static void format_time(struct timespec t, char *buf, size_t size)
{
	time_t sec = t.tv_sec;
	struct tm *tm = localtime(&sec);
	time_t now = time(NULL);

	if (now - sec > 15552000 || sec > now) {
		strftime(buf, size, "%b %e  %Y", tm);
	} else {
		strftime(buf, size, "%b %e %H:%M", tm);
	}
}

static struct timespec get_sort_time(const struct stat *st)
{
	if (flags & FLAG_ATIME)
		return st->st_atim;
	if (flags & FLAG_CTIME)
		return st->st_ctim;
	return st->st_mtim;
}

static int compare_files(const void *a, const void *b)
{
	const struct entry *fa = (const struct entry *)a;
	const struct entry *fb = (const struct entry *)b;
	int result = 0;

	if (flags & FLAG_NO_SORT)
		return 0;

	if (flags & FLAG_SIZE_SORT) {
		if (fa->st.st_size < fb->st.st_size)
			result = 1;
		else if (fa->st.st_size > fb->st.st_size)
			result = -1;
	} else if (flags & FLAG_TIME_SORT) {
		struct timespec ta = get_sort_time(&fa->st);
		struct timespec tb = get_sort_time(&fb->st);

		if (ta.tv_sec < tb.tv_sec ||
		    (ta.tv_sec == tb.tv_sec && ta.tv_nsec < tb.tv_nsec))
			result = 1;
		else if (ta.tv_sec > tb.tv_sec ||
			 (ta.tv_sec == tb.tv_sec && ta.tv_nsec > tb.tv_nsec))
			result = -1;
	}

	if (result == 0) {
		result = strcmp(fa->name, fb->name);
	}

	return (flags & FLAG_REVERSE) ? -result : result;
}

static void print_long_format(const struct entry *file)
{
	char perms[11];
	char time_buf[64];
	struct passwd *pw = NULL;
	struct group *gr = NULL;
	char *owner_name, *group_name;
	char owner_buf[32], group_buf[32];

	format_permissions(file->st.st_mode, perms);
	format_time(get_sort_time(&file->st), time_buf, sizeof(time_buf));

	if (flags & FLAG_NUMERIC) {
		snprintf(owner_buf, sizeof(owner_buf), "%u", file->st.st_uid);
		owner_name = owner_buf;
	} else {
		pw = getpwuid(file->st.st_uid);
		if (pw) {
			owner_name = pw->pw_name;
		} else if (file->st.st_uid == 0) {
			owner_name = "root";
		} else {
			snprintf(owner_buf, sizeof(owner_buf), "%u",
				 file->st.st_uid);
			owner_name = owner_buf;
		}
	}

	if (flags & FLAG_NUMERIC) {
		snprintf(group_buf, sizeof(group_buf), "%u", file->st.st_gid);
		group_name = group_buf;
	} else {
		gr = getgrgid(file->st.st_gid);
		if (gr) {
			group_name = gr->gr_name;
		} else if (file->st.st_gid == 0) {
			group_name = "root";
		} else {
			snprintf(group_buf, sizeof(group_buf), "%u",
				 file->st.st_gid);
			group_name = group_buf;
		}
	}

	if (flags & FLAG_INODE)
		printf("%8lu ", (unsigned long)file->st.st_ino);

	if (flags & FLAG_SIZE) {
		long blocks = file->st.st_blocks;
		if (flags & FLAG_KILO)
			blocks = (blocks + 1) / 2;
		printf("%6ld ", blocks);
	}

	printf("%s %3lu ", perms, (unsigned long)file->st.st_nlink);

	if (!(flags & FLAG_NO_GROUP)) {
		printf("%-8s %-8s ", owner_name, group_name);
	} else {
		printf("%-8s ", owner_name);
	}

	if (S_ISCHR(file->st.st_mode) || S_ISBLK(file->st.st_mode)) {
		printf("%3u, %3u ", major(file->st.st_rdev),
		       minor(file->st.st_rdev));
	} else {
		printf("%8ld ", (long)file->st.st_size);
	}

	printf("%s ", time_buf);

	if (S_ISLNK(file->st.st_mode) && file->is_link) {
		printf("%s -> %s", file->name, file->link_target);
	} else {
		printf("%s", file->name);
	}

	if (flags & FLAG_CLASSIFY) {
		char c = get_classify_char(file->st.st_mode);
		if (c)
			printf("%c", c);
	} else if (flags & FLAG_SLASH_DIR && S_ISDIR(file->st.st_mode)) {
		printf("/");
	}

	printf("\n");
}

static void print_simple_format(const struct entry *file)
{
	if (flags & FLAG_INODE)
		printf("%8lu ", (unsigned long)file->st.st_ino);

	if (flags & FLAG_SIZE) {
		long blocks = file->st.st_blocks;
		if (flags & FLAG_KILO)
			blocks = (blocks + 1) / 2;
		printf("%6ld ", blocks);
	}

	printf("%s", file->name);

	if (flags & FLAG_CLASSIFY) {
		char c = get_classify_char(file->st.st_mode);
		if (c)
			printf("%c", c);
	} else if (flags & FLAG_SLASH_DIR && S_ISDIR(file->st.st_mode)) {
		printf("/");
	}

	if (flags & FLAG_ONE_LINE) {
		printf("\n");
	} else {
		printf("  ");
	}
}

static int get_file_info(const char *path, const char *name, struct entry *info,
			 int follow_links)
{
	int ret;

	info->name = strdup(name);
	info->path = strdup(path);
	info->is_link = 0;

	if (follow_links || (flags & FLAG_FOLLOW_ALL)) {
		ret = stat(path, &info->st);
	} else {
		ret = lstat(path, &info->st);
	}

	if (ret != 0) {
		perror(path);
		return -1;
	}

	if (S_ISLNK(info->st.st_mode)) {
		ssize_t len = readlink(path, info->link_target,
				       sizeof(info->link_target) - 1);
		if (len != -1) {
			info->link_target[len] = '\0';
			info->is_link = 1;
		}
	}

	return 0;
}

static void free_file_info(struct entry *info)
{
	free(info->name);
	free(info->path);
}

static int lsdir(const char *path, int show_header)
{
	DIR *dir;
	struct dirent *entry;
	struct entry *files = NULL;
	int file_count = 0;
	int capacity = 0;
	char full_path[PATH_MAX];
	long total_blocks = 0;

	dir = opendir(path);
	if (!dir) {
		perror(path);
		return -1;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (!should_show_file(entry->d_name))
			continue;

		if (file_count >= capacity) {
			capacity = capacity ? capacity * 2 : 16;
			files = realloc(files, capacity * sizeof(struct entry));
			if (!files) {
				perror("realloc");
				closedir(dir);
				return -1;
			}
		}

		snprintf(full_path, sizeof(full_path), "%s/%s", path,
			 entry->d_name);

		if (get_file_info(full_path, entry->d_name, &files[file_count],
				  0) == 0) {
			if (flags & (FLAG_SIZE | FLAG_LONG)) {
				long blocks = files[file_count].st.st_blocks;
				if (flags & FLAG_KILO)
					blocks = (blocks + 1) / 2;
				total_blocks += blocks;
			}
			file_count++;
		}
	}
	closedir(dir);

	if (!(flags & FLAG_NO_SORT)) {
		qsort(files, file_count, sizeof(struct entry), compare_files);
	}

	if (show_header) {
		printf("\n%s:\n", path);
	}

	if ((flags & (FLAG_LONG | FLAG_SIZE)) && file_count > 0) {
		printf("total %ld\n", total_blocks);
	}

	for (int i = 0; i < file_count; i++) {
		if (flags & FLAG_LONG) {
			print_long_format(&files[i]);
		} else {
			print_simple_format(&files[i]);
		}

		if ((flags & FLAG_RECURSIVE) && S_ISDIR(files[i].st.st_mode) &&
		    strcmp(files[i].name, ".") != 0 &&
		    strcmp(files[i].name, "..") != 0) {
			lsdir(files[i].path, 1);
		}

		free_file_info(&files[i]);
	}

	if (!(flags & (FLAG_LONG | FLAG_ONE_LINE)) && file_count > 0) {
		printf("\n");
	}

	free(files);
	return 0;
}

static int ls(const char *path)
{
	struct entry info;
	struct stat st;
	int follow = (flags & FLAG_FOLLOW_ARG) || (flags & FLAG_FOLLOW_ALL);

	if ((follow ? stat(path, &st) : lstat(path, &st)) != 0) {
		fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
		return -1;
	}

	if (S_ISDIR(st.st_mode) && !(flags & FLAG_DIRECTORY)) {
		return lsdir(path, 0);
	}

	char *name = strrchr(path, '/');
	name = name ? name + 1 : (char *)path;

	if (get_file_info(path, name, &info, follow) == 0) {
		if (flags & FLAG_LONG) {
			print_long_format(&info);
		} else {
			print_simple_format(&info);
		}
		free_file_info(&info);
		return 0;
	}

	return -1;
}

int main(int argc, char **argv)
{
	int opt;
	int ret = 0;

	if (flags & FLAG_NO_SORT) {
		flags |= FLAG_ALL;
	}

	while ((opt = getopt(argc, argv, "AaClFHikLmnopqRrSstux1cdfg")) != -1) {
		switch (opt) {
		case 'A':
			flags |= FLAG_ALMOST_ALL;
			break;
		case 'a':
			flags |= FLAG_ALL;
			break;
		case 'C':
			flags |= FLAG_COLUMNS;
			break;
		case 'l':
			flags |= FLAG_LONG;
			break;
		case 'F':
			flags |= FLAG_CLASSIFY;
			break;
		case 'H':
			flags |= FLAG_FOLLOW_ARG;
			break;
		case 'i':
			flags |= FLAG_INODE;
			break;
		case 'k':
			flags |= FLAG_KILO;
			break;
		case 'L':
			flags |= FLAG_FOLLOW_ALL;
			break;
		case 'm':
			flags |= FLAG_COMMA;
			break;
		case 'n':
			flags |= FLAG_NUMERIC | FLAG_LONG;
			break;
		case 'o':
			flags |= FLAG_NO_GROUP | FLAG_LONG;
			break;
		case 'p':
			flags |= FLAG_SLASH_DIR;
			break;
		case 'q':
			flags |= FLAG_QUOTE;
			break;
		case 'R':
			flags |= FLAG_RECURSIVE;
			break;
		case 'r':
			flags |= FLAG_REVERSE;
			break;
		case 'S':
			flags |= FLAG_SIZE_SORT;
			break;
		case 's':
			flags |= FLAG_SIZE;
			break;
		case 't':
			flags |= FLAG_TIME_SORT;
			break;
		case 'u':
			flags |= FLAG_ATIME;
			break;
		case 'x':
			flags |= FLAG_ACROSS;
			break;
		case '1':
			flags |= FLAG_ONE_LINE;
			break;
		case 'c':
			flags |= FLAG_CTIME;
			break;
		case 'd':
			flags |= FLAG_DIRECTORY;
			break;
		case 'f':
			flags |= FLAG_NO_SORT | FLAG_ALL;
			break;
		case 'g':
			flags |= FLAG_LONG;
			break;
		case '?':
		default:
			usage();
			return 1;
		}
	}

	if (optind == argc) {
		ret = lsdir(".", 0);
	} else {
		for (int i = optind; i < argc; i++) {
			if (i > optind)
				printf("\n");
			if (ls(argv[i]) != 0) {
				ret = 1;
			}
		}
	}

	return ret;
}
