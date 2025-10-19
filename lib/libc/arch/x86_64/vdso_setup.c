#include <linux/auxvec.h>
#include <string.h>
#include <linux/elf.h>
#include <stddef.h>
#include <time.h>

int (*__vdso_clock_gettime)(int, struct timespec *) = NULL;
int (*__vdso_getcpu)(unsigned *, unsigned *, void *) = NULL;
int (*__vdso_time)(long *) = NULL;

static __inline void vdso_setup(Elf64_Ehdr *vdso_addr)
{
	Elf64_Phdr *phdr =
		(Elf64_Phdr *)((char *)vdso_addr + vdso_addr->e_phoff);

	for (int i = 0; i < vdso_addr->e_phnum; i++) {
		if (phdr[i].p_type == PT_DYNAMIC) {
			Elf64_Dyn *dyn = (Elf64_Dyn *)((char *)vdso_addr +
						       phdr[i].p_offset);
			const char *strtab = NULL;
			Elf64_Sym *symtab = NULL;
			for (; dyn->d_tag != DT_NULL; dyn++) {
				if (dyn->d_tag == DT_STRTAB) {
					strtab = (const char *)vdso_addr +
						 dyn->d_un.d_ptr;
				} else if (dyn->d_tag == DT_SYMTAB) {
					symtab =
						(Elf64_Sym *)((char *)vdso_addr +
							      dyn->d_un.d_ptr);
				}
			}

			if (strtab == NULL || symtab == NULL) {
				return;
			}

			Elf64_Sym *sym = symtab;
			while ((char *)sym < (char *)strtab) {
				if (sym->st_name != 0 && sym->st_value != 0) {
					const char *name =
						strtab + sym->st_name;
					if (strcmp(name,
						   "__vdso_clock_gettime") == 0)
						__vdso_clock_gettime =
							(void *)(vdso_addr +
								 sym->st_value);
					else if (strcmp(name,
							"__vdso_getcpu") == 0)
						__vdso_getcpu =
							(void *)(vdso_addr +
								 sym->st_value);
					else if (strcmp(name, "__vdso_time") ==
						 0)
						__vdso_time =
							(void *)(vdso_addr +
								 sym->st_value);
				}
				sym = (Elf64_Sym *)((char *)sym +
						    sizeof(Elf64_Sym));
			}
		}
	}
}