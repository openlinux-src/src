#include <unistd.h>

int main(void)
{
	write(STDOUT_FILENO, "\033[H\033[J", 6);
	return 0;
}
