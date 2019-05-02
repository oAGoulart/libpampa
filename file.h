#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>

#if defined WINDOWS
	#include <io.h>

	#if defined 64_BIT
		#define STAT _stat64
	#else
		#define STAT _stat32
	#endif

	#define MODE  _S_IREAD | _S_IWRITE
	#define OPEN  _open
	#define OFLAG _O_RDWR | _O_CREAT | _O_EXCL
	#define CLOSE _close
#else
	#include <fcntl.h>
	#include <unistd.h>

	#define STAT  stat
	#define MODE  0777
	#define OPEN  open
	#define OFLAG O_RDWR | O_CREAT | O_EXCL
	#define CLOSE close
#endif

typedef struct data
{
	uint64_t size;     /* buffer size without ending \0 */
	uint8_t* buffer;   /* data buffer */
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

/* free up data space in buffer */
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

/* load a block of data from file into buffer */
bool load_file_data(file_t* file, const uint64_t offset, const uint64_t count)
{
	bool result = false;

	if (file != NULL) {
		if (file->handle != NULL && offset <= file->size) {
			/* store current position indicator for later */
			uint64_t position = ftell(file->handle);

			if (!fseek(file->handle, offset, SEEK_SET)) {
				/* where the data starts on the file */
				file->offset = offset;

				/* maximum/given buffer size without end of line char */
				file->data.size = (offset + count > file->size) ? file->size - offset : count;

				/* allocate memory */
				file->data.buffer = (uint8_t*)realloc(file->data.buffer, (file->data.size + 1));

				/* prevents string overflow when using string.h functions */
				file->data.buffer[file->data.size] = '\0';

				/* read data from file */
				if (fread(file->data.buffer, 1, file->data.size, file->handle) == file->data.size)
					result = true;
				else
					unload_file_data(file);
			}

			/* restore file position indicator */
			fseek(file->handle, position, SEEK_SET);
		}
	}

	return result;
}

/* write a block of data from buffer into file */
bool replace_file_data(file_t* file, const uint64_t offset, const uint64_t count)
{
	bool result = false;

	if (file != NULL) {
		if (file->handle != NULL && file->data.buffer != NULL && offset <= file->size) {
			/* store current position indicator for later */
			uint64_t position = ftell(file->handle);

			if (!fseek(file->handle, offset, SEEK_SET)) {
				/* prevents buffer data overflow */
				uint64_t size = (count > file->data.size) ? file->data.size : count;

				/* write data to file */
				result = (fwrite(file->data.buffer, 1, size, file->handle) == size);
			}

			/* restore file position indicator */
			fseek(file->handle, position, SEEK_SET);
		}
	}

	return result;
}

/* write a block of data to the file buffer */
void replace_buffer_data(file_t* file, data_t* data)
{
	if (file != NULL && data != NULL) {
		if (file->data.buffer != NULL && data->buffer != NULL) {
			/* prevents file buffer data overflow */
			uint64_t size = (data->size > file->data.size) ? file->data.size : data->size;

			/* write data to file buffer */
			memcpy(file->data.buffer, data->buffer, size);

			/* updata file buffer size */
			file->data.size = size;
		}
	}
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

/* open file without loading any data from disk to memory */
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
			file->data.buffer = (uint8_t*)malloc(1);

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

bool create_file(char* filename)
{
	/* create file and store handle */
	int  file = OPEN(filename, OFLAG, MODE);

	return (file == -1) ? false : (!CLOSE(file));
}

#endif
