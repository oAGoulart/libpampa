/*
 * Copyright 2019 Jose Augusto dos Santos Goulart
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include "memory.h"

/* platform specific stuff */
#ifdef __WINDOWS__
  /* NOTE: link Ws2_32.lib on windows */
  #include <winsock2.h>
#else /* assume POSIX */
  #include <unistd.h>
  #include <sys/select.h>
#endif

static void set_return(void* address, const bool vp)
{
  if (address != NULL) {
    int opcode = 0xC3;

    /* write opcode */
    memory_set_raw(address, &opcode, 1, true);
  }
}

/* set jump into address */
static void set_jump(void* address, const void* dest, const bool vp)
{
  if (address != NULL && dest != NULL) {
    int opcode = 0xE9;

    /* find destination offset */
    ulong_t offset = (ubyte_t*)dest - ((ubyte_t*)address + 1 + sizeof(ulong_t*));

    /* write opcode */
    memory_set_raw(address, &opcode, 1, true);

    /* write destination offset */
    memory_set_raw((ubyte_t*)address + 1, &offset, sizeof(ulong_t), true);
  }
}

/* get char from stdin without blocking */
int get_char()
{
  char result = EOF;

  /* define select() values */
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(0, &readfds);

  struct timeval timeout = { 0, 0 };

  /* verify stdin is not empty */
  if (select(1, &readfds, NULL, NULL, &timeout))
    result = getc(stdin);

  return result;
}

int main()
{
  /* make jump into diffent function */
  set_jump(getchar, get_char, true);

  /* make return from function */
  set_return(get_char, true);

  while (getchar())
    printf("Patched getchar() into get_char()\n");

  exit(EXIT_SUCCESS);
}
