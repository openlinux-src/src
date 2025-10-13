int ffsl(long i)
{
	if (i == 0)
		return 0;

	int pos = 1;
	unsigned long u = (unsigned long)i;

	while ((u & 1UL) == 0UL) {
		u >>= 1;
		pos++;
	}
	return pos;
}
