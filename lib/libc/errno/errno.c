// TODO: Should return the address of the calling thread's `errno` storage.
// Currently, this is a stub implementation that returns the address of a
// static variable.
int *__errno(void)
{
	static int __thread_errno = 0;
	return &__thread_errno;
}
