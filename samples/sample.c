#include "../file.h"
#include <stdio.h>

/* entry point */
int main()
{
	/* open file */
	file_t* file = open_file("/home/agoulart/MEGAsync/Backup/calc.c");

	if (file != NULL) {
		printf("%llu\n", file->size);

		/* load file's data into memory */
		if (load_file_data(file, 0, file->size))
			printf("%s\n", file->data.buffer);

		printf("%llu\n", file->data.size);

		/* close file */
		close_file(file);
	}
}
