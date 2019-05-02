#include "../file.h"
#include <stdio.h>

/* entry point */
int main()
{
	/* create file */
	create_file("/home/agoulart/MEGAsync/Repositories/loader/samples/test.test");

	/* open file */
	file_t* file = open_file("/home/agoulart/MEGAsync/Repositories/loader/samples/test.test");

	if (file != NULL) {
		printf("%llu\n", file->size);

		/* load file's data into memory */
		if (load_file_data(file, 0, file->size))
			printf("%s\n", file->data.buffer);

		/* allocate memory */
		data_t data;
		data.size = 10;
		data.buffer = (uint8_t*)"0123456789";

		/* replace file buffer data */
		replace_buffer_data(file, &data);

		/* replace file data */
		replace_file_data(file, 0, file->size);

		printf("%llu\n", file->data.size);

		/* close file */
		close_file(file);
	}
}
