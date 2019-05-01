#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/stat.h>

#if defined WINDOWS_x86
	#define STAT _stat32
#elif defined WINDOWS_x64
	#define STAT _stat64
#else
	#define STAT stat
#endif

typedef struct data
{
	uint64_t size;     /* buffer size without ending \0 */
	char*    buffer;   /* data storage */
} data_t;

typedef struct file
{
	char*    path;     /* path to the file */
	FILE*    handle;   /* stdio file handle */
	uint64_t offset;   /* current location from the file origin */
	uint64_t size;     /* size of the file (not the buffer) */
	data_t   data;     /* buffer to store file data */
} file_t;

/* find the size of a file */
uint64_t find_file_size(const char* filename)
{
	uint64_t file_size = 0;
	struct STAT file_status;

	/* return 0 if successful */
	if (!STAT(filename, &file_status))
		file_size = file_status.st_size;

	return file_size;
}

/* free up data space in memory */
void unload_file_data(file_t* file)
{
	if (file != NULL) {
		if (file->data.buffer != NULL) {
			free(file->data.buffer);
			file->data.buffer = NULL;
		}

		file->data.size = 0;
	}
}

/* load a block of data from file into memory  */
bool load_file_data(file_t* file, const uint64_t offset, const uint64_t count)
{
	bool result = false;

	if (file->handle != NULL && offset <= file->size) {
		if (!fseek(file->handle, offset, SEEK_SET)) {
			/* where the data starts on the file */
			file->offset = offset;

			/* buffer size without end of line char */
			file->data.size = (offset + count > file->size) ? file->size - offset : count;

			/* allocate memory */
			file->data.buffer = (char*)realloc(file->data.buffer, (file->data.size + 1));

			/* prevent string overflow when using string.h functions */
			file->data.buffer[file->data.size] = '\0';

			/* read data from file */
			result = (fread(file->data.buffer, 1, file->data.size, file->handle) == file->data.size);
		}
	}

	return result;
}

/* close file and clean up memory */
void close_file(file_t* file)
{
	if (file != NULL) {
		if (file->handle != NULL)
			fclose(file->handle);

		/* free memory and clean pointers */
		unload_file_data(file);
		free(file);
		file = NULL;
	}
}

/* open file without loading any data from disk */
file_t* open_file(char* filename)
{
	file_t* file = NULL;

	/* initialize file struct */
	file = (file_t*)calloc(sizeof(*file), 1);

	/* asign values to struct */
	if (file != NULL) {
		file->path = filename;
		file->handle = fopen(file->path, "rb+");

		if (file->handle != NULL) {
			file->offset = 0;
			file->size = find_file_size(file->path);

			/* initialize the data buffer */
			file->data.size = 0;
			file->data.buffer = (char*)malloc(1);

			if (file->data.buffer != NULL)
				*file->data.buffer = '\0';
			else
				close_file(file);
		}
		else
			close_file(file);
	}

	return file;
}

#endif
