#!/bin/sh

rm -f tests/headers/output.txt

# Headers tests
echo "Running headers tests..."

f=0
i=0
n=$(ls -1 tests/headers/*.c | wc -l | tr -d ' ')
for file in tests/headers/*.c; do
    i=$((i + 1))
    echo "[$i/$n] $file"
    clang -nostdinc -Iinclude -fsyntax-only -o /dev/null "$file" 2> tests/headers/output.txt
    if [ $? -ne 0 ]; then
	echo "Test failed: $file"
	f=1
    fi
done

if [ $f -eq 0 ]; then
    echo "All headers tests passed!"
    rm -f tests/headers/output.txt
else
    echo "Some headers tests failed. See tests/headers/output.txt for details."
    exit 1
fi
