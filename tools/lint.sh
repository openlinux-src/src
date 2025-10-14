#!/bin/sh

find lib -name '*.c' | grep -v 'dtoa\|libm' \
	| xargs clang --analyze -Xanalyzer -analyzer-disable-checker \
		-Xanalyzer unix.Malloc \
		-nostdlib -nostdinc -nodefaultlibs -Iinclude -Ilib/libc/include -Ilib/libc/arch/x86_64/include -Ilib/libc/arch/x86_64 -target x86_64-linux-eabi -fmacro-backtrace-limit=0 --

rm -f *.plist
