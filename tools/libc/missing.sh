#!/bin/sh

LIBC_SYMBOLS=tools/libc/libc_defined.txt
LIBM_SYMBOLS=tools/libc/libm_defined.txt
INC_SYMBOLS=tools/libc/inc_defined.txt

llvm-nm lib/libc/libc.a 2> /dev/null | grep -v '__' | awk '{ print $3 }' | 
	sort | uniq | grep -v '^$' > $LIBC_SYMBOLS

llvm-nm lib/libm/libm.a 2> /dev/null | grep -v '__' | awk '{ print $3 }' | 
	sort | uniq | grep -v '^$' > $LIBM_SYMBOLS

find build/x86_64/sysroot/usr/include -name '*.h' -print0 | xargs -0 ctags -x | awk '{print $1}' |
	sort | uniq > $INC_SYMBOLS

cat "$LIBC_SYMBOLS" "$LIBM_SYMBOLS" "$INC_SYMBOLS" | sort | uniq > tools/libc/symbols.txt

N=$(wc -l < tools/libc/required.txt | awk '{print $1}')
n=$(comm -12 tools/libc/required.txt tools/libc/symbols.txt | wc -l)

comm -23 tools/libc/required.txt tools/libc/symbols.txt

echo "Symbols: $n/$N $(echo "scale=2; $n*100/$N" | bc -l)%"
