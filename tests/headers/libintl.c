// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/libintl.h.html

#include "core.h"

#include <libintl.h>

TEST(libintl_h)
{
	MACRO(TEXTDOMAIN_MAX);
	TYPE(locale_t);

	FUNCTION(bindtextdomain, FN(char *, const char *, const char *));
	FUNCTION(bind_textdomain_codeset,
		 FN(char *, const char *, const char *));
	FUNCTION(dcgettext, FN(char *, const char *, const char *, int));
	FUNCTION(dcgettext_l,
		 FN(char *, const char *, const char *, int, locale_t));
	FUNCTION(dcngettext, FN(char *, const char *, const char *,
				const char *, unsigned long, int));
	FUNCTION(dcngettext_l, FN(char *, const char *, const char *,
				  const char *, unsigned long, int, locale_t));
	FUNCTION(dgettext, FN(char *, const char *, const char *));
	FUNCTION(dgettext_l, FN(char *, const char *, const char *, locale_t));
	FUNCTION(dngettext, FN(char *, const char *, const char *, const char *,
			       unsigned long));
	FUNCTION(dngettext_l, FN(char *, const char *, const char *,
				 const char *, unsigned long, locale_t));
	FUNCTION(gettext, FN(char *, const char *));
	FUNCTION(gettext_l, FN(char *, const char *, locale_t));
	FUNCTION(ngettext,
		 FN(char *, const char *, const char *, unsigned long));
	FUNCTION(ngettext_l, FN(char *, const char *, const char *,
				unsigned long, locale_t));
	FUNCTION(textdomain, FN(char *, const char *));
}
