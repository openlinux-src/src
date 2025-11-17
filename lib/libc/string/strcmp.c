int strcmp(const char *s1, const char *s2)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	while (*p1 == *p2 && *p1 != '\0') {
		p1++;
		p2++;
	}

	return *p1 - *p2;
}
