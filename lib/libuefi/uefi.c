#include "uefi.h"

static void *__img = NULL;
static struct uefi_system_table *__systbl = NULL;
static struct uefi_loaded_image_protocol *__loaded_image = NULL;
static struct uefi_guid __loaded_image_protocol_guid =
	UEFI_LOADED_IMAGE_PROTOCOL_GUID;
static struct uefi_guid __device_path_protocol_guid =
	UEFI_DEVICE_PATH_PROTOCOL_GUID;

static struct uefi_boot_services *BS;

#define ELFMAG "\177ELF"

#define SELFMAG 4

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef struct {
	unsigned char e_ident[16]; /* Magic number and other info */
	Elf64_Half e_type;	   /* Object file type */
	Elf64_Half e_machine;	   /* Architecture */
	Elf64_Word e_version;	   /* Object file version */
	Elf64_Addr e_entry;	   /* Entry point virtual address */
	Elf64_Off e_phoff;	   /* Program header table file offset */
	Elf64_Off e_shoff;	   /* Section header table file offset */
	Elf64_Word e_flags;	   /* Processor-specific flags */
	Elf64_Half e_ehsize;	   /* ELF header size in bytes */
	Elf64_Half e_phentsize;	   /* Program header table entry size */
	Elf64_Half e_phnum;	   /* Program header table entry count */
	Elf64_Half e_shentsize;	   /* Section header table entry size */
	Elf64_Half e_shnum;	   /* Section header table entry count */
	Elf64_Half e_shstrndx;	   /* Section header string table index */
} Elf64_Ehdr;

typedef struct {
	Elf64_Word p_type;    /* Segment type */
	Elf64_Word p_flags;   /* Segment flags */
	Elf64_Off p_offset;   /* Segment file offset */
	Elf64_Addr p_vaddr;   /* Segment virtual address */
	Elf64_Addr p_paddr;   /* Segment physical address */
	Elf64_Xword p_filesz; /* Segment size in file */
	Elf64_Xword p_memsz;  /* Segment size in memory */
	Elf64_Xword p_align;  /* Segment alignment */
} Elf64_Phdr;

#define ET_EXEC	  2
#define EM_X86_64 62
#define PT_LOAD	  1
#define PF_X	  1
#define PF_W	  2
#define PF_R	  4

int errno = 0;

static uint16_t *utf8_to_char16(const char *s);
struct __tools __uefi_tools;
struct __tools *image = &__uefi_tools;

extern int main(void);

void *__load_elf(const char *path)
{
	uint64_t status;
	struct uefi_file_protocol *file_handle;
	struct uefi_filesystem_protocol *fs_proto;
	struct uefi_file_protocol *root;
	void *buffer = NULL;
	uint64_t buffer_size = 0;

	struct uefi_guid fs_guid = UEFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	status = BS->locate_protocol(&fs_guid, NULL, (void *)&fs_proto);
	if (UEFI_ERROR(status)) {
		write(2, "Failed to locate FS protocol\n", 29);
		return NULL;
	}

	write(1, "Loading ELF: ", 13);
	write(1, path, strlen(path));
	write(1, "\n", 1);

	write(1, "HERE1\n", 6);
	fs_proto->open_volume(fs_proto, &root);
	if (UEFI_ERROR(status)) {
		write(2, "Failed to open volume\n", 22);
		return NULL;
	}

	write(1, "Opened volume\n", 14);

	uint16_t *path_utf16 = utf8_to_char16(path);

	write(1, "Converted path to UTF-16\n", 25);
	status = root->open(root, &file_handle, path_utf16, UEFI_FILE_MODE_READ,
			    0);
	write(1, "Opened file\n", 12);
	free(path_utf16);
	if (UEFI_ERROR(status)) {
		write(2, "Failed to open file\n", 20);
		((struct { uint64_t (*close)(void *); } *)root)->close(root);
		return NULL;
	}

	write(1, "Opened ELF file\n", 16);

	// Get file size (use get_info with EFI_FILE_INFO_GUID)
	struct uefi_guid file_info_guid = UEFI_FILE_INFO_GUID;
	uint64_t info_size = 0;

	if (file_handle->revision != UEFI_FILE_PROTOCOL_LATEST_REVISION) {
		write(2, "Unsupported file protocol revision\n", 35);
		goto cleanup;
	}

	status = file_handle->get_info(file_handle, &file_info_guid, &info_size,
				       NULL);

	write(1, "Got file info size\n", 19);
	if (status == UEFI_BUFFER_TOO_SMALL) {
		void *info = malloc(info_size);
		status = file_handle->get_info(file_handle, &file_info_guid,
					       &info_size, info);
		write(1, "Got file info\n", 14);
		if (UEFI_ERROR(status)) {
			free(info);
			goto cleanup;
		}
		buffer_size =
			((struct { uint64_t file_size; } *)info)->file_size;
		free(info);
	} else {
		goto cleanup;
	}

	buffer = malloc(buffer_size);
	if (buffer == NULL) {
		write(2, "Malloc failed for ELF buffer\n", 29);
		goto cleanup;
	}

	write(1, "Allocated buffer for ELF\n", 25);
	status = file_handle->read(file_handle, &buffer_size, buffer);

	write(1, "Read ELF file\n", 14);
	if (UEFI_ERROR(status)) {
		write(2, "Read failed\n", 12);
		free(buffer);
		buffer = NULL;
		goto cleanup;
	}

	// Parse ELF
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)buffer;
	if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0 ||
	    ehdr->e_type != ET_EXEC || ehdr->e_machine != EM_X86_64) {
		write(2, "Invalid ELF\n", 12);
		free(buffer);
		buffer = NULL;
		goto cleanup;
	}

	// Load program headers
	for (Elf64_Half i = 0; i < ehdr->e_phnum; i++) {
		Elf64_Phdr *phdr =
			(Elf64_Phdr *)((uint8_t *)buffer + ehdr->e_phoff +
				       i * ehdr->e_phentsize);
		if (phdr->p_type != PT_LOAD)
			continue;

		// Allocate memory at p_paddr (use AllocatePool or Pages if fixed)
		void *dest = NULL;
		status = __systbl->boot_services->allocate_pages(
			UEFI_ALLOCATE_ADDRESS, UEFI_LOADER_DATA,
			(phdr->p_memsz + 0xFFF) / 0xFFF, (uint64_t *)&dest);
		if (UEFI_ERROR(status) ||
		    dest != (void *)phdr
				    ->p_paddr) { // If fixed address required; else use ANY_PAGES
			write(2, "Alloc failed\n", 13);
			free(buffer);
			buffer = NULL;
			goto cleanup;
		}

		// Copy filesz, zero the rest (BSS)
		memcpy(dest, buffer + phdr->p_offset, phdr->p_filesz);
		memset(dest + phdr->p_filesz, 0,
		       phdr->p_memsz - phdr->p_filesz);
	}

	// Relocations (if dynamic ELF, add code for Elf64_Rela; assume static for simplicity)

	void *entry = (void *)ehdr->e_entry;

	free(buffer);

cleanup:
	((struct { uint64_t (*close)(void *); } *)file_handle)
		->close(file_handle);
	((struct { uint64_t (*close)(void *); } *)root)->close(root);

	return entry; // Return entry point or NULL on failure
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *p1 = s1;
	const unsigned char *p2 = s2;

	for (size_t i = 0; i < n; i++) {
		if (p1[i] != p2[i]) {
			return (int)(p1[i] - p2[i]);
		}
	}

	return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	unsigned char *d = dest;
	const unsigned char *s = src;

	for (size_t i = 0; i < n; i++) {
		d[i] = s[i];
	}

	return dest;
}

static uint16_t *utf8_to_char16(const char *s)
{
	if (s == NULL) {
		write(2, "utf8_to_char16: input string is NULL\n", 37);
		return NULL;
	}

	size_t len = 0;
	for (const unsigned char *p = (const unsigned char *)s; *p;) {
		if ((*p & 0x80) == 0)
			p += 1;
		else if ((*p & 0xE0) == 0xC0)
			p += 2;
		else if ((*p & 0xF0) == 0xE0)
			p += 3;
		else if ((*p & 0xF8) == 0xF0)
			p += 4;
		else
			p++;
		len++;
	}

	uint16_t *out;
	if ((out = malloc((len + 1) * sizeof(uint16_t))) == NULL) {
		write(2, "utf8_to_char16: memory allocation failed\n", 41);
		return NULL;
	}

	size_t i = 0;
	for (const unsigned char *p = (const unsigned char *)s; *p;) {
		uint32_t codepoint;

		if ((*p & 0x80) == 0) {
			codepoint = *p++;
		} else if ((*p & 0xE0) == 0xC0) {
			codepoint = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
			p += 2;
		} else if ((*p & 0xF0) == 0xE0) {
			codepoint = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) |
				    (p[2] & 0x3F);
			p += 3;
		} else if ((*p & 0xF8) == 0xF0) {
			codepoint = ((*p & 0x07) << 18) |
				    ((p[1] & 0x3F) << 12) |
				    ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
			p += 4;
		} else {
			p++;
			continue;
		}

		if (codepoint <= 0xFFFF) {
			out[i++] = (uint16_t)codepoint;
		} else {
			codepoint -= 0x10000;
			out[i++] = (uint16_t)(0xD800 |
					      ((codepoint >> 10) & 0x3FF));
			out[i++] = (uint16_t)(0xDC00 | (codepoint & 0x3FF));
		}
	}

	out[i] = 0;
	return out;
}

char *itoa_safe(int value, char *buf, size_t bufsize)
{
	if (buf == NULL || bufsize < 2)
		return NULL; // Sanity check

	char *p = buf + bufsize - 1;
	*p = '\0';
	size_t len = 0;

	int is_negative = (value < 0);
	if (is_negative) {
		unsigned int uval = (unsigned int)(-value);
		do {
			if (++len >= bufsize - 1)
				return NULL;
			*--p = '0' + (uval % 10);
			uval /= 10;
		} while (uval != 0);
		if (++len >= bufsize)
			return NULL;
		*--p = '-';
	} else {
		unsigned int uval = (unsigned int)value;
		do {
			if (++len >= bufsize - 1)
				return NULL;
			*--p = '0' + (uval % 10);
			uval /= 10;
		} while (uval != 0);
	}

	return p;
}

void *memset(void *s, int c, size_t n)
{
	unsigned char *p = s;
	while (n--) {
		*p++ = (unsigned char)c;
	}
	return s;
}

int mbtowc(wchar_t *wc, const char *s, size_t n)
{
	wchar_t arg;
	int ret = 1;
	if (s == NULL || *s == '\0') {
		return 0;
	}

	arg = (wchar_t)*s;

	if ((*s & 128) != 0) {
		if ((*s & 32) == 0 && n > 0) {
			arg = ((*s & 0x1F) << 6) | (*(s + 1) & 0x3F);
			ret = 2;
		} else if ((*s & 16) == 0 && n > 1) {
			arg = ((*s & 0xF) << 12) | ((*(s + 1) & 0x3F) << 6) |
			      (*(s + 2) & 0x3F);
			ret = 3;
		} else if ((*s & 8) == 0 && n > 2) {
			arg = ((*s & 0x7) << 18) | ((*(s + 1) & 0x3F) << 12) |
			      ((*(s + 2) & 0x3F) << 6) | (*(s + 3) & 0x3F);
			ret = 4;
		} else {
			return -1;
		}
	}
	if (wc) {
		*wc = arg;
	}
	return ret;
}

int wctomb(char *s, wchar_t u)
{
	int ret = 0;

	if (u < 0x80) {
		*s = u;
		ret = 1;
	} else if (u < 0x800) {
		*(s + 0) = ((u >> 6) & 0x1F) | 0xC0;
		*(s + 1) = (u & 0x3F) | 0x80;
		ret = 2;
	} else {
		*(s + 0) = ((u >> 12) & 0x0F) | 0xE0;
		*(s + 1) = ((u >> 6) & 0x3F) | 0x80;
		*(s + 2) = (u & 0x3F) | 0x80;
		ret = 3;
	}

	return ret;
}

char *strchr(const char *s, int c)
{
	while (*s) {
		if (*s == (char)c)
			return (char *)s;
		s++;
	}

	if (c == 0) {
		return (char *)s;
	}

	return NULL;
}

ssize_t write(int fd, const void *buf, size_t count)
{
	const char *s = buf;
	uint16_t wbuf[256];
	struct simple_text_output_interface *out;
	size_t i = 0, total = 0;

	if (fd != 1 && fd != 2)
		return -1;
	out = (fd == 1) ? __systbl->con_out : __systbl->std_err;

	while (i < count) {
		size_t wpos = 0;

		while (i < count && wpos < (sizeof(wbuf) / sizeof(*wbuf)) - 1) {
			if (s[i] == '\n') {
				wbuf[wpos++] = L'\r';
				wbuf[wpos++] = L'\n';
				i++;
				continue;
			}

			wchar_t wc;
			int n = mbtowc(&wc, s + i, count - i);
			if (n <= 0)
				break;
			wbuf[wpos++] = (uint16_t)wc;
			i += n;
		}
		wbuf[wpos] = 0;

		out->output_string(out, wbuf);
		total += i - total;
	}

	return total;
}

ssize_t read(int fd, void *buf, size_t count)
{
	struct simple_input_interface *in;
	if (fd != 0)
		return -1;

	in = __systbl->con_in;
	char *out = buf;
	size_t total = 0;

	while (total < count) {
		struct uefi_input_key key;
		uint64_t status;

		do {
			status = in->read_key_stroke(in, &key);
		} while (status == UEFI_NOT_READY);

		if (key.unicode_char == L'\r' || key.unicode_char == L'\n') {
			if (total < count)
				out[total++] = '\n';
			continue;
		}

		if (key.unicode_char) {
			int n = wctomb(out + total, key.unicode_char);
			if (n < 0)
				continue;
			total += n;
		}
	}

	return total;
}

void *malloc(size_t size)
{
	void *ptr;
	uint64_t status;

	status = __systbl->boot_services->allocate_pool(
		__loaded_image ? __loaded_image->image_data_type :
				 UEFI_LOADER_DATA,
		size, &ptr);

	if (UEFI_ERROR(status) || ptr == NULL) {
		write(2, "malloc: memory allocation failed\n", 33);
		return NULL;
	}

	return ptr;
}

void free(void *ptr)
{
	__systbl->boot_services->free_pool(ptr);
}

struct __attribute__((packed)) uefi_device_path_header {
	uint8_t type;
	uint8_t subtype;
	uint16_t length;
};

struct __attribute__((packed)) uefi_file_path {
	struct uefi_device_path_header header;
	uint16_t path_name[]; // elastyczna tablica
};

struct __attribute__((packed)) uefi_device_path_end {
	uint8_t type;
	uint8_t subtype;
	uint16_t length;
};

size_t strlen(const char *s)
{
	size_t len = 0;
	while (s[len] != '\0') {
		len++;
	}
	return len;
}

void *__load_image(const char *path)
{
	uint64_t status;
	void *image_handle = NULL;
	uint16_t *path_utf16;
	size_t path_len;
	size_t file_node_size, base_size = 0, last_file_len = 0, total_size;
	void *buf;

	path_utf16 = utf8_to_char16(path);
	if (path_utf16 == NULL) {
		write(2, "Failed to convert path to UTF-16\n", 33);
		return NULL;
	}

	write(1, "Loading image: ", 15);
	write(1, path, strlen(path));
	write(1, "\n", 1);

	for (path_len = 0; path_utf16[path_len]; path_len++)
		;

	file_node_size = sizeof(struct uefi_device_path_header) +
			 (path_len + 1) * sizeof(uint16_t);

	// Base DevicePath is the loader's full file_path
	struct uefi_device_path *base_dp = __loaded_image->file_path;

	// Calculate base_size (full length excluding END)
	struct uefi_device_path *current = base_dp;
	while (!IS_DEVICE_PATH_END(current)) {
		size_t node_len = DEVICE_PATH_NODE_LENGTH(current);
		base_size += node_len;
		if (current->type == 4 && current->subtype == 4) {
			last_file_len =
				node_len; // Track the last file node length
		}
		current = (void *)NEXT_DEVICE_PATH_NODE(current);
	}

	if (last_file_len == 0) {
		write(2, "No file node in base path\n", 26);
		free(path_utf16);
		return NULL;
	}

	// Base directory size = full base_size - last_file_len
	size_t dir_size = base_size - last_file_len;

	// Total = directory + new file + END (4 bytes)
	total_size = dir_size + file_node_size + 4;

	buf = malloc(total_size);
	if (buf == NULL) {
		write(2, "Failed to allocate memory for device path\n", 42);
		free(path_utf16);
		return NULL;
	}

	// Copy the directory part
	memcpy(buf, base_dp, dir_size);

	// Append new file node
	struct uefi_file_path *file =
		(struct uefi_file_path *)((uint8_t *)buf + dir_size);
	file->header.type = 4;	  // MEDIA_DEVICE_PATH
	file->header.subtype = 4; // MEDIA_FILEPATH_DP
	file->header.length = (uint16_t)file_node_size;
	for (size_t i = 0; i <= path_len; i++) {
		file->path_name[i] = path_utf16[i];
	}

	// Append END
	struct uefi_device_path_end *end =
		(struct uefi_device_path_end *)((uint8_t *)file +
						file_node_size);
	end->type = END_DEVICE_PATH_TYPE;
	end->subtype = END_ENTIRE_DEVICE_PATH_SUBTYPE;
	end->length = 4;

	free(path_utf16);

	status = __systbl->boot_services->load_image(0, __img, buf, NULL, 0,
						     &image_handle);

	free(buf);

	if (status != 0) {
		write(2, "Failed to load image\n", 21);
		write(2, "UEFI status: ", 13);
		char buf[32];
		char *status_str = itoa_safe((int)(status & 0x7FFFFFFFFFFFFFFF),
					     buf, sizeof(buf)); // Positive code
		if (status_str) {
			write(2, status_str, strlen(status_str));
		}
		write(2, "\n", 1);
		return NULL;
	}

	return image_handle;
}

uint64_t __image_unload(void *image_handle)
{
	if (image_handle == NULL)
		return (uint64_t)-1;

	return __systbl->boot_services->unload_image(image_handle);
}

uint64_t _start(void *img_handle, struct uefi_system_table *sys_table)
{
	__img = img_handle;
	__systbl = sys_table;

	BS = __systbl->boot_services;

	uint64_t status = BS->handle_protocol(img_handle,
					      &__loaded_image_protocol_guid,
					      (void **)&__loaded_image);

	if (UEFI_ERROR(status)) {
		write(2, "Failed to open LoadedImage protocol\n", 36);
		goto halt;
	}

	image->load = __load_elf;

	return main();
halt:
	while (1)
		__asm__ __volatile__("hlt");
}
