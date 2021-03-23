/*
 * Copyright 2019-2021 Jose Augusto dos Santos Goulart
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
#include "log.h"
#include "miniz/miniz.c"

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
  #include <sys/types.h>

  #define MEM_UNPROT PROT_READ | PROT_WRITE | PROT_EXEC
#endif

/**
 * Memory allocation data
 */
typedef struct data_s
{
  /** buffer size */
  size_t   size;
  /** data location */
  ubyte_t* address;
} data_t;

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************
 * Free a block of data.
 *
 * @param data data struct reference
 **********************************************************/
void data_free(data_t* data)
{
  if (data != NULL) {
    if (data->address != NULL) {
      free(data->address);
      data->address = NULL;
    }

    data->size = 0;
  }
}

/**********************************************************
 * Allocate a block of data.
 *
 * @param data data struct reference with information for the
 *             allocation
 * @param size new buffer size
 *
 * @return bool whether or not data was allocated
 **********************************************************/
bool data_alloc(data_t* data, const size_t count)
{
  void* temp_ptr = NULL;
  if ((temp_ptr = realloc(data->address, count)) != NULL) {
    data->address = (ubyte_t*)temp_ptr;
    data->size = count;
    return true;
  }

  LOG("Warning: Could not reallocate memory for data.");
  return false;
}

/**********************************************************
 * Copy a block of data to another.
 *
 * @param source data to be copied
 * @param destination where to store data
 *
 * @return bool whether or not data was copied
 **********************************************************/
bool data_copy(data_t* source, data_t* destination)
{
  if (source != NULL && destination != NULL) {
    if (data_alloc(destination, source->size)) {
      memcpy(destination->address, source->address, source->size);
      return true;
    }
  }

  LOG("Warning: Could not copy source into destination.");
  return false;
}

/**********************************************************
 * Append a block of data to another.
 *
 * @param source data to be appended
 * @param destination where to append data
 *
 * @return bool whether or not data was appended
 **********************************************************/
bool data_append(data_t* source, data_t* destination)
{
  if (source != NULL && destination != NULL) {
    size_t count = destination->size;
    if (data_alloc(destination, count + source->size)) {
      memcpy(destination->address + count, source->address, source->size);
      return true;
    }
  }

  LOG("Warning: Could not append source into destination.");
  return false;
}

/**********************************************************
 * Append a byte to data buffer.
 *
 * @param destination where to append data
 *
 * @return bool whether or not data was appended
 **********************************************************/
bool data_append_byte(data_t* destination, const ubyte_t byte)
{
  if (destination != NULL) {
    if (data_alloc(destination, destination->size + 1)) {
      *(destination->address + destination->size - 1) = byte;
      return true;
    }
  }

  LOG("Warning: Could not append byte to destinations.");
  return false;
}

/**********************************************************
 * Compress source data into destination.
 *
 * @param source data to be compressed
 * @param destination where to store output
 *
 * @return bool whether or not operation was successful
 **********************************************************/
bool zlib_deflate(data_t* source, data_t* destination)
{
  z_stream stream;
  memset(&stream, 0, sizeof(stream));
  stream.next_in = source->address;
  stream.avail_in = (unsigned int)source->size;
  stream.next_out = destination->address;
  stream.avail_out = (unsigned int)destination->size;

  if (deflateInit(&stream, Z_DEFLATED) != Z_OK) {
    LOG("Warning: Could not init zlib deflation.");
    return false;
  }
  if (deflate(&stream, Z_FINISH) != Z_OK) {
    LOG("Warning: Could not deflate zlib stream.");
    return false;
  }

  return (deflateEnd(&stream) == Z_OK);
}

/**********************************************************
 * Decompress source data into destination.
 *
 * @param source data to be decompressed
 * @param destination where to store output
 *
 * @return bool whether or not operation was successful
 **********************************************************/
bool zlib_inflate(data_t* source, data_t* destination)
{
  z_stream stream;
  memset(&stream, 0, sizeof(stream));
  stream.next_in = source->address;
  stream.avail_in = (unsigned int)source->size;
  stream.next_out = destination->address;
  stream.avail_out = (unsigned int)destination->size;

  if (inflateInit(&stream) != Z_OK) {
    LOG("Warning: Could not init zlib inflation.");
    return false;
  }
  if (inflate(&stream, Z_FINISH) != Z_OK) {
    LOG("Warning: Could not inflate zlib stream.");
    return false;
  }

  return (inflateEnd(&stream) == Z_OK);
}

/**********************************************************
 * Set a block of system memory protection.
 *
 * @param address block of memory start address
 * @param size size of the block of memory
 * @param mode protection mode to be set
 **********************************************************/
void memory_set_protection(const void* address, const size_t size, const int mode)
{
  if (address != NULL && size > 0) {
#ifdef __WINDOWS__
    DWORD old_mode;
    VirtualProtect((LPVOID)address, size, mode, &old_mode);
#else
    long page_sz = sysconf(_SC_PAGESIZE);
    void* page_ptr = (long*)((long)address & ~(page_sz - 1));
    mprotect(page_ptr, size, mode);
#endif
  }
}

/**********************************************************
 * Get a block of system memory protection.
 *
 * @param address block of memory start address
 *
 * @return int system memory protection mode
 **********************************************************/
int memory_get_protection(const void* address)
{
  int result = 0;

  if (address != NULL) {
#ifdef __WINDOWS__
    MEMORY_BASIC_INFORMATION page_info;
    if (VirtualQuery(address, &page_info, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
      result = page_info.Protect;
#else
    FILE* maps = fopen("/proc/self/maps", "r");
    if (maps != NULL) {
      ulong_t block[2] = { 0, 0 }; /* block of memory addresses */
      char perms[5];               /* set of permissions */

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
      fclose(maps);
    }
#endif
  }

  return result;
}

/**********************************************************
 * Write a block of system memory.
 *
 * @param address block of memory start address
 * @param data data to be written
 * @param size size of the data
 * @param vp is virtual protection enabled?
 **********************************************************/
void memory_set_raw(void* address, const void* data, const size_t size, const bool vp)
{
  if (address != NULL && data != NULL && size > 0) {
    if (vp) {
      int old_mode = memory_get_protection(address);

      memory_set_protection(address, size, MEM_UNPROT);
      memcpy(address, data, size);
      memory_set_protection(address, size, old_mode);
    }
    else
      memcpy(address, data, size);
  }
}

#ifndef __WINDOWS__
/**********************************************************
 * Get the base address of a process.
 *
 * @param pid process id
 *
 * @return ulong_t base address
 **********************************************************/
ulong_t process_get_base_address(const pid_t pid)
{
  ulong_t base_addr = 0x0; /* base address */
  char path[FILENAME_MAX]; /* path to file */

  if (sprintf(path, "/proc/%d/maps", pid) >= 0) {
    FILE* maps = fopen(path, "r");

    if (maps != NULL) {
  #ifdef __X86_ARCH__
      fscanf(maps, "%x-%*x %*s %*x %*d:%*d %*d %*s\n", &base_addr);
  #else
      fscanf(maps, "%lx-%*x %*s %*x %*d:%*d %*d %*s\n", &base_addr);
  #endif
      fclose(maps);
    }
  }

  return base_addr;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _memory_h_ */
