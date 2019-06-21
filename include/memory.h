/*
 * Copyright 2019 José Augusto dos Santos Goulart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _memory_h_
#define _memory_h_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

/* define platform specific stuff */
#ifdef __WINDOWS__
	/* NOTE: link Kernel32.lib on windows */
	#ifndef WIN32_LEAN_AND_MEAN
 	#define WIN32_LEAN_AND_MEAN
 	#endif
	#include <Windows.h>
	#include <Memoryapi.h>

	#define MEM_UNPROT PAGE_EXECUTE_READWRITE
#else /* assume POSIX */
	#include <unistd.h>
	#include <sys/mman.h>

	#define MEM_UNPROT PROT_READ | PROT_WRITE | PROT_EXEC
#endif

/* define structs */
typedef struct data_s
{
	size_t size;      /* buffer size without ending nul char */
	ubyte_t* address; /* data location */
} data_t;

#ifdef __cplusplus
extern "C" {
#endif

/* free data buffer memory */
void data_free_buffer(data_t* data)
{
	if (data->address != NULL) {
		free(data->address);
		data->address = NULL;
	}

	data->size = 0;
}

/* allocate data buffer memory */
bool data_alloc_buffer(data_t* data)
{
	bool result = false;

	if (data != NULL) {
		/* allocate memory */
		data->address = realloc(data->address, (data->size + 1));

		if (data->address != NULL) {
			/* prevents memory overflow when using string.h functions */
			data->address[data->size] = '\0';

			result = true;
		}
	}

	return result;
}

/* set memory protection */
void memory_set_protection(const void* address, const size_t size, const int mode)
{
	if (address != NULL && size > 0) {
#ifdef __WINDOWS__
		int old_mode;
		VirtualProtect((LPVOID)address, size, mode, &old_mode);
#else
		/* get size of pages */
		long page_sz = sysconf(_SC_PAGESIZE);

		/* find page pointer */
		void* page_ptr = (long*)((long)address & ~(page_sz - 1));

		/* no previous mode to be returned */
		mprotect(page_ptr, size, mode);
#endif
	}
}

/* get memory protection */
int memory_get_protection(const void* address)
{
	int result = 0;

	if (address != NULL) {
#ifdef __WINDOWS__
		MEMORY_BASIC_INFORMATION page_info;
		if (VirtualQuery(address, &page_info, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
			result = page_info.Protect;
#else
		FILE* maps = fopen("/proc/self/maps", "r"); /* currently mapped memory regions */

		if (maps != NULL) {
			ulong_t block[2] = { 0, 0 };                /* block of memory addresses */
			char perms[5];                              /* set of permissions */

			/* parse file lines */
	#ifdef __X86_ARCH__
			while (fscanf(maps, "%x-%x %4s %*x %*d:%*d %*d %*s\n", &block[0], &block[1], perms) == 3) {
	#else
			while (fscanf(maps, "%lx-%lx %4s %*x %*d:%*d %*d %*s\n", &block[0], &block[1], perms) == 3) {
	#endif
				if (block[0] <= (ulong_t)address && block[1] >= (ulong_t)address) {
					result |= (perms[0] == 'r') ? PROT_READ : PROT_NONE;  /* can be readed */
					result |= (perms[1] == 'w') ? PROT_WRITE : PROT_NONE; /* can be written */
					result |= (perms[2] == 'x') ? PROT_EXEC : PROT_NONE;  /* can be executed */
					break;
				}
			}

			/* close file */
			fclose(maps);
		}
#endif
	}

	return result;
}

/* write data into memory */
void memory_set_raw(void* address, const void* data, const size_t size, const bool vp)
{
	if (address != NULL && data != NULL && size > 0) {
		if (vp) {
			/* store memory protection for later */
			int old_mode = memory_get_protection(address);

			/* remove virtual protection */
			memory_set_protection(address, size, MEM_UNPROT);

			/* write data into memory */
			memcpy(address, data, size);

			/* reset virtual protection */
			memory_set_protection(address, size, old_mode);
		}
		else
			memcpy(address, data, size);
	}
}

#ifdef __cplusplus
}
#endif

#endif /* _memory_h_ */
