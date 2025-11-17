#include <atomic.h>
#include <features.h>
#include <libc.h>
#include <malloc.h>
#include <linux/errno.h>
#include <stdatomic.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

struct page *__malloc_pvec = NULL;

static __inline uint32_t get_size_class(size_t size)
{
	uintptr_t minblock_count = (size + (16 - 1)) / 16;

	if (size <= (16 * 64)) {
		return (uint32_t)(minblock_count ? minblock_count : 1);
	}

	const uint32_t most_significant_bit =
		(uint32_t)(63 - (int)__builtin_clzll(minblock_count));

	const uint32_t subclass_bits =
		(minblock_count >> (most_significant_bit - 2)) & 0x03;

	return (uint32_t)((most_significant_bit << 2) + subclass_bits) + 41;
}

weak void *malloc(size_t size);

void *malloc(size_t size)
{
	if (size == 0)
		return NULL;

	LIBC_LOCK(libc.lock.malloc);

	uint32_t class_index = get_size_class(size);
	if (class_index >=
	    sizeof(global_size_class) / sizeof(global_size_class[0])) {
		LIBC_UNLOCK(libc.lock.malloc);
		return NULL;
	}
	const struct class *cls = &global_size_class[class_index];

	struct page *p = __malloc_pvec;
	while (p) {
		if (p->flags == PAGE_SMALL && cls->size <= 16 * 64) {
			LIBC_LOCK(p->lock);
			if (p->block.used < p->block.count &&
			    p->block.size == cls->size) {
				for (uint32_t i = 0; i < p->block.count; i++) {
					int byte_index = i / 8;
					int bit_index = i % 8;
					if (!(p->bitmap[byte_index] &
					      (1 << bit_index))) {
						p->bitmap[byte_index] |=
							(1 << bit_index);
						p->block.used++;
						LIBC_UNLOCK(p->lock);
						LIBC_UNLOCK(libc.lock.malloc);
						if (p->heap == NULL)
							return NULL;
						return p->heap +
						       (i * p->block.size);
					}
				}
			}
			LIBC_UNLOCK(p->lock);
		} else if (p->flags == PAGE_MEDIUM && cls->size > 16 * 64 &&
			   cls->size <= 16 * 8192) {
			LIBC_LOCK(p->lock);
			if (p->block.used < p->block.count &&
			    p->block.size == cls->size) {
				for (uint32_t i = 0; i < p->block.count; i++) {
					int byte_index = i / 8;
					int bit_index = i % 8;
					if (!(p->bitmap[byte_index] &
					      (1 << bit_index))) {
						// Mark block as used
						p->bitmap[byte_index] |=
							(1 << bit_index);
						p->block.used++;
						LIBC_UNLOCK(p->lock);
						LIBC_UNLOCK(libc.lock.malloc);
						if (p->heap == NULL)
							return NULL;
						return p->heap +
						       (i * p->block.size);
					}
				}
			}
			LIBC_UNLOCK(p->lock);
		} else if (p->flags == PAGE_LARGE && cls->size > 16 * 8192) {
			LIBC_LOCK(p->lock);
			if (p->block.used < p->block.count &&
			    p->block.size == cls->size) {
				// Find free block
				for (uint32_t i = 0; i < p->block.count; i++) {
					int byte_index = i / 8;
					int bit_index = i % 8;
					if (!(p->bitmap[byte_index] &
					      (1 << bit_index))) {
						p->bitmap[byte_index] |=
							(1 << bit_index);
						p->block.used++;
						LIBC_UNLOCK(p->lock);
						LIBC_UNLOCK(libc.lock.malloc);

						if (p->heap == NULL)
							return NULL;
						return p->heap +
						       (i * p->block.size);
					}
				}
			}
			LIBC_UNLOCK(p->lock);
		}
		p = p->next;
	}

	size_t page_size;
	enum { PAGE_TYPE_SMALL, PAGE_TYPE_MEDIUM, PAGE_TYPE_LARGE } page_type;

	if (cls->size <= 16 * 64) {
		page_size = SMALL_PAGE_SIZE;
		page_type = PAGE_TYPE_SMALL;
	} else if (cls->size <= 16 * 8192) {
		page_size = MEDIUM_PAGE_SIZE;
		page_type = PAGE_TYPE_MEDIUM;
	} else {
		page_size = LARGE_PAGE_SIZE;
		page_type = PAGE_TYPE_LARGE;
	}

	size_t bitmap_size = (cls->count + 7) / 8;
	void *mem = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (mem == MAP_FAILED) {
		LIBC_UNLOCK(libc.lock.malloc);
		return NULL;
	}

	struct page *new_page = (struct page *)mem;
	new_page->flags = (page_type == PAGE_TYPE_SMALL)  ? PAGE_SMALL :
			  (page_type == PAGE_TYPE_MEDIUM) ? PAGE_MEDIUM :
							    PAGE_LARGE;

	new_page->block.size = cls->size;
	new_page->block.used = 0;
	new_page->block.count = cls->count;
	new_page->bitmap = (uint8_t *)mem + sizeof(struct page);
	memset(new_page->bitmap, 0, bitmap_size);
	new_page->heap = (uint8_t *)mem + sizeof(struct page) + bitmap_size;

	if (new_page->heap == NULL || new_page->bitmap == NULL) {
		munmap(mem, page_size);
		LIBC_UNLOCK(libc.lock.malloc);
		return NULL;
	}
	atomic_flag_clear(&new_page->lock);
	new_page->next = __malloc_pvec;
	new_page->prev = NULL;

	if (__malloc_pvec) {
		__malloc_pvec->prev = new_page;
	}

	__malloc_pvec = new_page;

	// Mark the first block as used and return it
	new_page->bitmap[0] |= 1;
	new_page->block.used = 1;

	LIBC_UNLOCK(libc.lock.malloc);

	return new_page->heap;
}
