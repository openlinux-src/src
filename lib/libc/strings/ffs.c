int ffs(int i)
{
	if (i == 0)
		return 0;

	int pos = 1;
	unsigned int u = (unsigned int)i;

	while ((u & 1U) == 0U) {
		u >>= 1;
		pos++;
	}
	return pos;
}
