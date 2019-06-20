#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/memory.h"

/* platform specific stuff */
#ifdef __WINDOWS__
	/* NOTE: link Ws2_32.lib on windows */
	#include <winsock2.h>
#else /* assume POSIX */
	#include <unistd.h>
	#include <sys/select.h>
#endif

/* set jump into address */
static void set_jump(void* address, const void* dest, const bool vp)
{
	if (address != NULL && dest != NULL) {
		int opcode = 0xE9;

		/* find destination offset */
		ulong_t offset = (ulong_t*)dest - ((ulong_t*)address + 1 + sizeof(ulong_t*));

		/* write opcode */
		memory_set_raw(address, &opcode, 1, true);

		/* write destination offset */
		memory_set_raw((ulong_t*)address + 1, &offset, sizeof(ulong_t*), true);
	}
}

/* get char from stdin without blocking */
static int get_char()
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
	ubyte_t* addr = (void*)&getchar;

	if (getchar())
		set_jump(&getchar, &get_char, true);

	printf("%x %x %x %x %x\n%x\n", *addr, *(addr + 1), *(addr + 2), *(addr + 3), *(addr + 4), *(int*)&getchar);

	while (1) {
		printf("Let's hook this baby! %c\n", getchar());
	}
}
