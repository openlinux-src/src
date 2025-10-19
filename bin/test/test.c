#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

#define TEST_BUF_SIZE 512

// Simple test that shows what we expect vs what we get
void test_format(const char *desc, const char *expected, const char *fmt, ...)
{
	char actual[TEST_BUF_SIZE];
	va_list ap;

	va_start(ap, fmt);
	int len = vsnprintf(actual, sizeof(actual), fmt, ap);
	va_end(ap);

	printf("=== %s ===\n", desc);
	printf("Format:   \"%s\"\n", fmt);
	printf("Expected: [%s]\n", expected);
	printf("Actual:   [%s] (%d chars)\n", actual, len);

	if (strcmp(expected, actual) == 0) {
		printf("✅ PASS\n\n");
	} else {
		printf("❌ FAIL\n\n");
	}
}

// Debug individual format specifiers
void debug_format_char(char spec, int value, const char *expected)
{
	char fmt[4] = { '%', spec, '\0' };
	char actual[32];

	snprintf(actual, sizeof(actual), fmt, value);
	printf("%%%c with %d: expected='%s' actual='%s' %s\n", spec, value,
	       expected, actual, strcmp(expected, actual) == 0 ? "✅" : "❌");
}

int main(void)
{
	printf("=== DEBUG printf implementation ===\n\n");

	// Debug test: Simplest possible cases to isolate the issue
	printf("--- Ultra Simple Debug Tests ---\n");
	char simple_buf[64];

	// Test each format one by one
	snprintf(simple_buf, sizeof(simple_buf), "%d", 42);
	printf("%%d result: '%s' (should be '42')\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%u", 42);
	printf("%%u result: '%s' (should be '42')\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%o", 8);
	printf("%%o result: '%s' (should be '10' - 8 in octal)\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%x", 15);
	printf("%%x result: '%s' (should be 'f' - 15 in hex)\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%X", 15);
	printf("%%X result: '%s' (should be 'F' - 15 in HEX)\n", simple_buf);

	printf("\n--- Checking format character detection ---\n");
	// Debug: manually check what happens with different formats
	snprintf(simple_buf, sizeof(simple_buf), "%c", 'A');
	printf("%%c result: '%s' (should be 'A')\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%s", "hello");
	printf("%%s result: '%s' (should be 'hello')\n", simple_buf);

	printf("\n--- Testing if any unsigned formats work ---\n");
	snprintf(simple_buf, sizeof(simple_buf), "%u", 123);
	printf("%%u with 123: '%s'\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%o", 64);
	printf("%%o with 64: '%s' (should be '100')\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%x", 255);
	printf("%%x with 255: '%s' (should be 'ff')\n", simple_buf);

	printf("\n--- Switch Statement Debug ---\n");
	// Test if the issue is in switch statement execution
	// Let's see what happens with known working vs broken formats

	printf("Testing known working formats:\n");
	snprintf(simple_buf, sizeof(simple_buf), "%d", -5);
	printf("  %%d with -5: '%s' (signed should work)\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%c", 'Z');
	printf("  %%c with 'Z': '%s' (char should work)\n", simple_buf);

	printf("Testing broken formats:\n");
	snprintf(simple_buf, sizeof(simple_buf), "%u", 42);
	printf("  %%u with 42: '%s' (if this is '42', unsigned case works)\n",
	       simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%o", 8);
	printf("  %%o with 8: '%s' (if this is '10', octal case works)\n",
	       simple_buf);

	printf("Testing edge case - what if we use %%i vs %%d:\n");
	snprintf(simple_buf, sizeof(simple_buf), "%i", 42);
	printf("  %%i with 42: '%s' (should be same as %%d)\n", simple_buf);

	printf("\n--- Hash Flag Isolation ---\n");
	// Test hash flags in isolation
	snprintf(simple_buf, sizeof(simple_buf), "%#d", 42);
	printf("  %%#d with 42: '%s' (# should be ignored for %%d)\n",
	       simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%#o", 0);
	printf("  %%#o with 0: '%s' (# should be ignored for 0)\n", simple_buf);

	snprintf(simple_buf, sizeof(simple_buf), "%#o", 1);
	printf("  %%#o with 1: '%s' (should be '01' if # works)\n", simple_buf);

	printf("\n--- Format Character Detection Debug ---\n");
	// Test what format character is actually being detected
	char debug_buf[100];

	// We need to hack into vfprintf to see what fmt_char is
	// For now, let's test if the issue is with *(ptr-1)
	printf("Testing format character detection:\n");

	// Test each format with a unique value to see the pattern
	snprintf(debug_buf, sizeof(debug_buf), "%o", 7); // 7 octal = 7
	printf("  %%o with 7: '%s' (should be '7' in both decimal and octal)\n",
	       debug_buf);

	snprintf(debug_buf, sizeof(debug_buf), "%x", 10); // 10 decimal = a hex
	printf("  %%x with 10: '%s' (should be 'a' if hex, '10' if decimal)\n",
	       debug_buf);

	snprintf(debug_buf, sizeof(debug_buf), "%X", 10); // 10 decimal = A hex
	printf("  %%X with 10: '%s' (should be 'A' if hex, '10' if decimal)\n",
	       debug_buf);

	printf("\n--- Direct vfprintf Test ---\n");
	// Let's test vfprintf directly to see if the issue is there
	printf("Testing vfprintf directly:\n");

	FILE *stderr_file = stderr; // Use stderr as our test stream
	fprintf(stderr, "Direct fprintf test - %%d: %d\n", 42);
	fprintf(stderr, "Direct fprintf test - %%u: %u\n", 42);
	fprintf(stderr, "Direct fprintf test - %%o: %o\n", 8);
	fprintf(stderr, "Direct fprintf test - %%x: %x\n", 15);

	printf("(Check stderr output above)\n");

	printf("\n--- Raw Buffer Test ---\n");
	// Test with a raw buffer to see exactly what gets written
	char raw_buf[100];
	int len;

	len = snprintf(raw_buf, sizeof(raw_buf), "Test: %d %u %o %x", 42, 42, 8,
		       15);
	printf("Raw buffer result: '%s' (len=%d)\n", raw_buf, len);
	printf("Should be: 'Test: 42 42 10 f'\n");

	// Character by character analysis
	printf("Character analysis: ");
	for (int i = 0; i < len && i < 50; i++) {
		if (raw_buf[i] >= 32 && raw_buf[i] <= 126) {
			printf("%c", raw_buf[i]);
		} else {
			printf("[%d]", raw_buf[i]);
		}
	}
	printf("\n");

	printf("\n");

	// Test basic format characters individually
	printf("--- Basic Format Characters ---\n");
	debug_format_char('d', 42, "42");
	debug_format_char('u', 42, "42");
	debug_format_char('o', 64, "100"); // 64 decimal = 100 octal
	debug_format_char('x', 255, "ff"); // 255 decimal = ff hex
	debug_format_char('X', 255, "FF"); // 255 decimal = FF hex
	printf("\n");

	// Test hash flags
	printf("--- Hash Flag Tests ---\n");
	char buf[32];
	snprintf(buf, sizeof(buf), "%#o", 64);
	printf("%%#o with 64: expected='0100' actual='%s' %s\n", buf,
	       strcmp("0100", buf) == 0 ? "✅" : "❌");

	snprintf(buf, sizeof(buf), "%#x", 255);
	printf("%%#x with 255: expected='0xff' actual='%s' %s\n", buf,
	       strcmp("0xff", buf) == 0 ? "✅" : "❌");

	snprintf(buf, sizeof(buf), "%#X", 255);
	printf("%%#X with 255: expected='0XFF' actual='%s' %s\n", buf,
	       strcmp("0XFF", buf) == 0 ? "✅" : "❌");
	printf("\n");

	// Comprehensive tests with expected results
	test_format("Signed integers", "42 -42 0", "%d %i %d", 42, -42, 0);

	test_format("Unsigned integers", "42 0 4294967295", "%u %u %u", 42u, 0u,
		    4294967295u);

	test_format("Octal and Hex", "100 0100 ff 0xff FF 0XFF",
		    "%o %#o %x %#x %X %#X", 64, 64, 255, 255, 255, 255);

	test_format("Hash flag edge cases", "0 0 0 0 0 0",
		    "%o %#o %x %#x %X %#X", 0, 0, 0, 0, 0, 0);

	test_format("Plus/space/zero flags", "+42 -42  42 00042",
		    "%+d %+d % d %05d", 42, -42, 42, 42);

	test_format("Left alignment", "123   |123   |0xabc |",
		    "%-6d|%-6u|%-#6x|", 123, 123, 0xabc);

	test_format("Width and precision", "       123|00123|     00123|",
		    "%10d|%.5d|%10.5d|", 123, 123, 123);

	test_format("Variable width/precision", "    0123|", "%*.*d|", 8, 4,
		    123);

	test_format("Length modifiers", "127 32000 123456 1234567890123",
		    "%hhd %hd %ld %lld", (char)127, (short)32000, 123456L,
		    1234567890123LL);

	test_format("Size/ptrdiff", "12345 -42", "%zd %td", (size_t)12345,
		    (ptrdiff_t)-42);

	int x = 1234;
	// Note: pointer values are system-specific, so we'll just test format
	printf("=== Pointer test ===\n");
	printf("Format: \"%%p\"\n");
	printf("Result: [%p]\n", &x);
	printf("Should start with 0x and be hexadecimal\n\n");

	test_format("Character and string", "A hello ", "%c %s %s", 'A',
		    "hello", "");

	test_format("String precision", "abc ", "%.3s %.0s", "abcdef", "xyz");

	test_format("Percent literal", "100% done!", "100%% done!");

	test_format("Basic floats", "0.000000 1.500000 -123.456000", "%f %f %f",
		    0.0, 1.5, -123.456);

	test_format("Float width/precision", "      3.14|3.14159|  -3.142|",
		    "%10.2f|%.5f|%+8.3f|", 3.14159, 3.14159, -3.14159);

	test_format("Exponential format", "1.234568e+03 1.234000E-05", "%e %E",
		    1234.5678, 0.00001234);

	test_format("General format", "1.23457e+06 1.234e-05", "%g %g",
		    1234567.0, 0.00001234);

	test_format("Hex float", "0x1.921f9f01b866ep+1 0X1.921F9F01B866EP+1",
		    "%a %A", 3.14159, 3.14159);

	test_format("NaN and Infinity", "nan inf -inf", "%g %g %g", NAN,
		    INFINITY, -INFINITY);

	test_format("Zero padding with negatives",
		    "-000000042 0000000042 000000002a", "%010d %010u %010x",
		    -42, 42, 42);

	test_format("Complex mixed format",
		    "Int:+00042 Hex:0x00002a Float: 3.142 Str:hello      Done",
		    "Int:%+06d Hex:%#08x Float:% .3f Str:%-10.5s Done", 42,
		    0x2A, 3.14159, "hello world");

	printf("=== Test Complete ===\n");
	printf("Look for ❌ FAIL markers to identify issues\n");

	return 0;
}
