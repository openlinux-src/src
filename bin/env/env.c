#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	extern char **environ;
	char **ptr;

	ptr = environ;
	while (*ptr) {
		write(STDOUT_FILENO, *ptr, strlen(*ptr));
		write(STDOUT_FILENO, "\n", 1);
		ptr++;
	}
}
