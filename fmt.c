static char *
shquote(argv)
	char **argv;
{
	long arg_max;
	char **p, *dst, *src;
	static char *buf = NULL;

	if (buf == NULL) {
		if ((arg_max = sysconf(_SC_ARG_MAX)) == -1)
			errx(1, "sysconf _SC_ARG_MAX failed");
		if ((buf = malloc((4 * arg_max)  +  1)) == NULL)
			errx(1, "malloc failed");
	}

	if (*argv == 0) {
		buf[0] = 0;
		return (buf);
	}
	dst = buf;
	for (p = argv; (src = *p++) != 0; ) {
		if (*src == 0)
			continue;
		strvis(dst, src, VIS_NL | VIS_CSTYLE);
		while (*dst)
			dst++;
		*dst++ = ' ';
	}
	/* Chop off trailing space */
	if (dst != buf)
		dst--;
	*dst = '\0';
	return (buf);
}

