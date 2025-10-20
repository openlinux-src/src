// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/ftw.h.html

#include "core.h"

#include <ftw.h>

TEST(ftw_h)
{
	TYPE(struct FTW);
	STRUCT_MEMBER(struct FTW, int, base);
	STRUCT_MEMBER(struct FTW, int, level);

	MACRO(FTW_F);
	MACRO(FTW_D);
	MACRO(FTW_DNR);
	MACRO(FTW_DP);
	MACRO(FTW_NS);
	MACRO(FTW_SL);
	MACRO(FTW_SLN);

	MACRO(FTW_PHYS);
	MACRO(FTW_MOUNT);
	MACRO(FTW_XDEV);
	MACRO(FTW_DEPTH);
	MACRO(FTW_CHDIR);
}
