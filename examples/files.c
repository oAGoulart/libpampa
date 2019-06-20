#include "../include/file.h"
#include <stdio.h>

/* entry point */
int main()
{
	/* create file */
	file_create("/home/agoulart/MEGAsync/Repositories/loader/samples/test.test");

	/* open file */
	file_t* file;

	if (file_open(&file, "/home/agoulart/MEGAsync/Repositories/loader/samples/test.test")) {
		printf("%lu\n", (unsigned long)file->size);

		/* load file's data into memory */
		if (file_load(file, 0, file->size))
			printf("%s\n", file->buffer.address);

		/* allocate memory */
		data_t data;
		data.size = 10;
		data.address = (ubyte_t*)"0123456789";

		/* replace file buffer data */
		file_replace_buffer(file, &data);

		/* replace file data */
		file_write(file, 0, file->size);

		printf("%lu\n", (unsigned long)file->buffer.size);

		/* close file */
		file_close(&file);
	}
}
