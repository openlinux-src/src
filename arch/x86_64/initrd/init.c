#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <syscall.h>
#include <dirent.h>

#include <linux/fs.h>

struct rootfs {
	char *dev;
	int flags;
};

void pr_info(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fputs("init: ", stderr);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

int get_rootfs(struct rootfs *root)
{
	int fd;
	ssize_t len;
	char buf[1024];
	char arg[256];

	root->dev = NULL;
	root->flags = 0;

	fd = open("/proc/cmdline", O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	len = read(fd, buf, sizeof(buf) - 1);
	if (len < 0) {
		close(fd);
		return -1;
	}
	buf[len] = '\0';
	close(fd);

	// Read cmdline by words
	// not using strtok
	char *p = buf;
	while (*p) {
		while (*p == ' ')
			p++;

		if (*p == '\0')
			break;

		char *start = p;
		while (*p && *p != ' ')
			p++;
		size_t word_len = p - start;

		if (strncmp(start, "root=", 5) == 0) {
			size_t dev_len = word_len - 5;
			root->dev = strndup(start + 5, dev_len);
		} else if (strncmp(start, "ro", 2) == 0) {
			root->flags |= MS_RDONLY;
		} else if (strncmp(start, "rw", 2) == 0) {
			root->flags &= ~MS_RDONLY;
		}
	}

	return 0;
}

#define ERROR_MKDIR_DEV	 32
#define ERROR_MOUNT_DEV	 33
#define ERROR_MKDIR_PROC 34
#define ERROR_MOUNT_PROC 35
#define ERROR_MKDIR_SYS	 36
#define ERROR_MOUNT_SYS	 37

void ls(char *path)
{
	struct dirent *entry;
	DIR *dp = opendir(path);
	if (dp == NULL) {
		perror("opendir");
		return;
	}

	printf("%s:\n", path);
	while ((entry = readdir(dp)) != NULL) {
		printf("%s\n", entry->d_name);
	}

	closedir(dp);
}

int main(void)
{
	struct rootfs root;
	char *const argv[] = { "/bin/init", NULL };

	if (mkdir("/dev", 0755) < 0 && errno != EEXIST) {
		perror("mkdir /dev");
		return -1;
	}

	if (mount("devtmpfs", "/dev", "devtmpfs", 0, NULL) < 0) {
		perror("mount devtmpfs");
		return -1;
	}

	if (mkdir("/proc", 0755) < 0 && errno != EEXIST) {
		perror("mkdir /proc");
		return -1;
	}

	if (mount("proc", "/proc", "proc", 0, NULL) < 0) {
		perror("mount proc");
		return -1;
	}

	/* Get root filesystem info from kernel cmdline */
	if (get_rootfs(&root) < 0) {
		pr_info("Failed to get root filesystem info\n");
		return -1;
	}

	pr_info("Root device: %s\n", root.dev);

	if (mount(root.dev, "/root", "ext4", root.flags, NULL) < 0) {
		perror("init: mount rootfs");
		return ERROR_MOUNT_DEV;
	}

	static const struct {
		const char *path;
		unsigned long
			mount_flags; /* Flags to apply if SWITCH_ROOT_RECURSIVE_RUN is unset */
		unsigned long
			mount_flags_recursive_run; /* Flags to apply if SWITCH_ROOT_RECURSIVE_RUN is set (0 if shall be skipped) */
	} transfer_table[] = {
		{ "/dev", MS_BIND | MS_REC, MS_BIND | MS_REC },
		{ "/sys", MS_BIND | MS_REC, MS_BIND | MS_REC },
		{ "/proc", MS_BIND | MS_REC, MS_BIND | MS_REC },
		{ "/run", MS_BIND, MS_BIND | MS_REC },
	};

	if (mkdir("/root/dev", 0755) < 0) {
		perror("init: chdir /root");
		return -1;
	}

	if (mount("/dev", "/root/dev", NULL, MS_BIND | MS_REC, NULL) < 0) {
		perror("init: mount --bind /dev");
		return -1;
	}

	if (mkdir("/root/proc", 0755) < 0) {
		perror("init: chdir /root");
		return -1;
	}

	if (mount("/proc", "/root/proc", NULL, MS_BIND | MS_REC, NULL) < 0) {
		perror("init: mount --bind /proc");
		return -1;
	}

	if (mkdir("/root/sys", 0755) < 0) {
		perror("init: chdir /root");
		return -1;
	}

	if (mount("sysfs", "/root/sys", "sysfs", 0, NULL) < 0) {
		perror("init: mount --bind /sys");
		return -1;
	}

	if (syscall(chroot, "/root") < 0) {
		perror("init: chroot /root");
		return -1;
	}

	chdir("/");

	execve("/bin/init", argv, NULL);
	perror("init: execve /bin/init");

	return 0;
}
