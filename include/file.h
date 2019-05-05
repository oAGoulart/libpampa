/*
 * Copyright 2019 Augusto Goulart
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

#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* handle data types size */
#define UBYTE uint8_t

#ifdef __NR_stat64
	#define ULONG uint64_t
#else
	#define ULONG uint32_t
#endif

/* handle platforms calls */
#ifdef WINDOWS
	#include <io.h>

	#define STAT  _stat64                        /* may be defined as _stat */
	#define MODE  _S_IREAD | _S_IWRITE
	#define OPEN  _open
	#define OFLAG _O_RDWR | _O_CREAT | _O_EXCL
	#define CLOSE _close
#else
	#include <fcntl.h>

	#define STAT  stat                           /* always stat64 on kernel 2.4+ */
	#define MODE  0777
	#define OPEN  open
	#define OFLAG O_RDWR | O_CREAT | O_EXCL
	#define CLOSE close
#endif

/* structs */
typedef struct data
{
	size_t size;     /* buffer size without ending \0 */
	UBYTE* buffer;   /* data buffer */
} data_t;

typedef struct file
{
	char*  path;     /* path to the file */
	FILE*  handle;   /* stdio file handle */
	ULONG  offset;   /* current location from the file origin */
	size_t size;     /* size of the file (not the buffer) */
	data_t data;     /* buffer to store file data */
} file_t;

/* free buffer memory */
void free_data_memory(data_t* data)
{
	if (data->buffer != NULL) {
		free(data->buffer);
		data->buffer = NULL;
	}

	data->size = 0;
}

/* allocate buffer memory */
bool alloc_data_memory(data_t* data)
{
	bool result = false;

	if (data != NULL) {
		/* allocate memory */
		data->buffer = (UBYTE*)realloc(data->buffer, (data->size + 1));

		if (data->buffer != NULL) {
			/* prevents string overflow when using string.h functions */
			data->buffer[data->size] = '\0';

			result = true;
		}
	}

	return result;
}

/* find the size of a file */
ULONG find_file_size(const char* filename)
{
	ULONG file_size = 0;
	struct STAT file_status;

	/* return 0 if successful */
	if (!STAT(filename, &file_status))
		file_size = file_status.st_size;

	return file_size;
}

/* load a block of data from file into buffer */
bool load_file_data(file_t* file, const ULONG offset, const ULONG count)
{
	bool result = false;

	if (file != NULL) {
		if (file->handle != NULL && offset <= file->size) {
			/* store current position indicator for later */
			ULONG position = ftell(file->handle);

			if (!fseek(file->handle, offset, SEEK_SET)) {
				/* where the data starts on the file */
				file->offset = offset;

				/* maximum/given buffer size without end of line char */
				file->data.size = (offset + count > file->size) ? file->size - offset : count;

				/* allocate memory */
				if (alloc_data_memory(&file->data)) {
					/* read data from file */
					if (fread(file->data.buffer, 1, file->data.size, file->handle) == file->data.size)
						result = true;
					else
						free_data_memory(&file->data);
				}
				else
					free_data_memory(&file->data);
			}

			/* restore file position indicator */
			fseek(file->handle, position, SEEK_SET);
		}
	}

	return result;
}

/* write a block of data from buffer into file */
bool replace_file_data(file_t* file, const ULONG offset, const ULONG count)
{
	bool result = false;

	if (file != NULL) {
		if (file->handle != NULL && file->data.buffer != NULL && offset <= file->size) {
			/* store current position indicator for later */
			ULONG position = ftell(file->handle);

			if (!fseek(file->handle, offset, SEEK_SET)) {
				/* prevents buffer data overflow */
				ULONG size = (count > file->data.size) ? file->data.size : count;

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
			ULONG size = (data->size > file->data.size) ? file->data.size : data->size;

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
		free_data_memory(&file->data);
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
			file->data.buffer = (UBYTE*)malloc(1);

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

/* create a new file if one doesn't exist */
bool create_file(char* filename)
{
	/* create file and store handle */
	int  file = OPEN(filename, OFLAG, MODE);

	/* return result and free handle */
	return (file == -1) ? false : (!CLOSE(file));
}

#endif