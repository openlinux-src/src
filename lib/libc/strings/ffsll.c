int ffsll(long long i)
{
	if (i == 0)
		return 0;

	int pos = 1;
	unsigned long long u = (unsigned long long)i;

	while ((u & 1ULL) == 0ULL) {
		u >>= 1;
		pos++;
	}
	return pos;
}
