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
#include <errno.h>

/* add 64 bits file support */
#ifdef X64_FILES
	#define _FILE_OFFSET_BITS 64
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* define data types and platform calls */
#define UBYTE uint8_t

#ifdef WINDOWS
	#include <io.h>
	#include <share.h>

	#ifdef X64_FILES
		#define NLONG int64_t

		#define STAT  _stat64     /* NOTE: may be defined as _stat on Win32 */
		#define FTELL _ftelli64
		#define FSEEK _fseeki64
	#else
		#define NLONG int32_t

		#define STAT  _stat32
		#define FTELL ftell
		#define FSEEK fseek
	#endif

	#define OFF_T NLONG

	#define MODE  _S_IREAD | _S_IWRITE
	#define OPEN(handle, filename, flag, mode) (_sopen_s(handle, filename, flag, _SH_DENYNO, mode))
	#define OFLAG _O_RDWR | _O_CREAT | _O_EXCL
	#define CLOSE _close
	#define FOPEN fopen_s
#else /* asume POSIX */
	#include <unistd.h>

	#ifdef X64_FILES
		#define NLONG uint64_t
		#define OFF_T NLONG

		#define FTELL ftello
		#define FSEEK fseeko
	#else
		#define NLONG uint32_t
		#define OFF_T int32_t

		#define FTELL ftell
		#define FSEEK fseek
	#endif

	#define STAT  stat /* always stat64 on Linux kernel 2.4+ */
	#define MODE  0777
	#define OPEN(handle, filename, flag, mode) (((*handle = open(filename, flag, mode)) == -1) ? errno : false)
	#define OFLAG O_RDWR | O_CREAT | O_EXCL
	#define CLOSE close
	#define FOPEN(file, filename, mode) (((*file = fopen(filename, mode)) == NULL) ? errno : false)
#endif

/* define structs */
typedef struct data
{
	size_t size;    /* buffer size without ending nul char */
	UBYTE* address; /* data location */
} data_t;

typedef struct file
{
	char*  path;   /* file path */
	FILE*  handle; /* stdio file handle */
	OFF_T  offset; /* current location from the file origin */
	size_t size;   /* size of the file in disk */
	data_t buffer; /* buffer to load file data */
} file_t;

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
		data->address = (UBYTE*)realloc(data->address, (data->size + 1));

		if (data->address != NULL) {
			/* prevents memory overflow when using string.h functions */
			data->address[data->size] = '\0';

			result = true;
		}
	}

	return result;
}

/* find the size of a file */
NLONG file_find_size(const char* filename)
{
	NLONG file_size = 0;
	struct STAT file_status;

	/* return 0 if successful */
	if (!STAT(filename, &file_status))
		file_size = file_status.st_size;

	return file_size;
}

/* load a block of data from file into buffer */
bool file_load(file_t* file, const OFF_T offset, const OFF_T count)
{
	bool result = false;

	if (file != NULL) {
		if (file->handle != NULL && (size_t)offset <= file->size) {
			/* store current position indicator for later */
			OFF_T position = FTELL(file->handle);

			if (!FSEEK(file->handle, offset, SEEK_SET)) {
				/* where the data starts on the file */
				file->offset = offset;

				/* maximum/given buffer size without end of line char */
				file->buffer.size = ((size_t)(offset + count) > file->size) ? file->size - offset : count;

				/* allocate memory */
				if (data_alloc_buffer(&file->buffer)) {
					/* read data from file */
					if (fread(file->buffer.address, 1, file->buffer.size, file->handle) == file->buffer.size)
						result = true;
					else
						data_free_buffer(&file->buffer);
				}
				else
					data_free_buffer(&file->buffer);
			}

			/* restore file position indicator */
			FSEEK(file->handle, position, SEEK_SET);
		}
	}

	return result;
}

/* write a block of data from buffer into file */
bool file_write(file_t* file, const OFF_T offset, const OFF_T count)
{
	bool result = false;

	if (file != NULL) {
		if (file->handle != NULL && file->buffer.address != NULL && (size_t)offset <= file->size) {
			/* store current position indicator for later */
			OFF_T position = FTELL(file->handle);

			if (!FSEEK(file->handle, offset, SEEK_SET)) {
				/* prevents buffer data overflow */
				size_t size = ((size_t)count > file->buffer.size) ? file->buffer.size : count;

				/* write data to file */
				result = (fwrite(file->buffer.address, 1, size, file->handle) == size);
			}

			/* restore file position indicator */
			FSEEK(file->handle, position, SEEK_SET);
		}
	}

	return result;
}

/* write a block of data to the file buffer */
void file_replace_buffer(file_t* file, data_t* data)
{
	if (file != NULL && data != NULL) {
		if (file->buffer.address != NULL && data->address != NULL) {
			/* prevents file buffer data overflow */
			size_t size = (data->size > file->buffer.size) ? file->buffer.size : data->size;

			/* write data to file buffer */
			memcpy(file->buffer.address, data->address, size);

			/* updata file buffer size */
			file->buffer.size = size;
		}
	}
}

/* close file and clean up memory */
void file_close(file_t** file)
{
	if (*file != NULL) {
		if ((*file)->handle != NULL)
			fclose((*file)->handle);

		/* free memory and clean pointers */
		data_free_buffer(&(*file)->buffer);
		free(*file);
		*file = NULL;
	}
}

/* open file without loading any data from disk to memory */
bool file_open(file_t** file, char* filename)
{
	bool result = false;

	/* initialize file struct */
	*file = (file_t*)calloc(sizeof(**file), 1);

	/* asign values to struct */
	if (*file != NULL) {
		(*file)->path = filename;

		if (!FOPEN(&(*file)->handle, (*file)->path, "rb+")) {
			(*file)->offset = 0;
			(*file)->size = file_find_size((*file)->path);

			/* initialize the data buffer */
			(*file)->buffer.size = 0;
			(*file)->buffer.address = (UBYTE*)malloc(1);

			if ((*file)->buffer.address != NULL) {
				*(*file)->buffer.address = '\0';
				result = true;
			}
			else
				file_close(file);
		}
		else
			file_close(file);
	}

	return result;
}

/* create a new file if one doesn't exist */
bool file_create(char* filename)
{
	int file;

	/* return result and free handle */
	return (OPEN(&file, filename, OFLAG, MODE)) ? false : (!CLOSE(file));
}

#endif
