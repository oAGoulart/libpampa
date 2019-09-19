/*
 * Copyright 2019 José Augusto dos Santos Goulart
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

#ifndef _file_h_
#define _file_h_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "types.h"
#include "memory.h"

/* add 64 bit file support */
#ifdef X64_FILES
	#define _FILE_OFFSET_BITS 64

	#define OFF_T int64_t
#else
	#define OFF_T int32_t
	#define FTELL ftell
	#define FSEEK fseek
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* define platform specific stuff */
#ifdef __WINDOWS__
	#include <io.h>
	#include <share.h>

	#ifdef X64_FILES
		#define STAT  _stat64   /* NOTE: this may be defined as _stat on Win32 */
		#define FTELL _ftelli64
		#define FSEEK _fseeki64
	#else
		#define STAT  _stat32
	#endif

	#define MODE  _S_IREAD | _S_IWRITE
	#define OPEN(handle, filename, flag, mode) (_sopen_s(handle, filename, flag, _SH_DENYNO, mode))
	#define OFLAG _O_RDWR | _O_CREAT | _O_EXCL
	#define CLOSE _close
	#define FOPEN fopen_s
	#define FSCANF fscanf_s
#else /* assume POSIX */
	#include <unistd.h>

	#ifdef X64_FILES
		#define FTELL ftello
		#define FSEEK fseeko
	#endif

	#define STAT  stat /* always stat64 on Linux kernel 2.4+ */
	#define MODE  0777
	#define OPEN(handle, filename, flag, mode) (((*handle = open(filename, flag, mode)) == -1) ? errno : false)
	#define OFLAG O_RDWR | O_CREAT | O_EXCL
	#define CLOSE close
	#define FOPEN(file, filename, mode) (((*file = fopen(filename, mode)) == NULL) ? errno : false)
	#define FSCANF fscanf
#endif

typedef struct file_s
{
	char*  path;   /* file path */
	FILE*  handle; /* stdio file handle */
	OFF_T  offset; /* where buffer data starts */
	size_t size;   /* size of the file in disk */
	data_t buffer; /* buffer to load file data */
} file_t;

#ifdef __cplusplus
extern "C" {
#endif

/* find the size of a file */
OFF_T file_find_size(const file_t* file)
{
	if (file != NULL) {
		struct STAT file_status;

		/* return file size */
		if (!STAT(file->path, &file_status))
			return file_status.st_size;
	}

	return 0;
}

/* read a block of data from file into buffer */
bool file_read(file_t* file, const OFF_T offset, const OFF_T count, const bool change_indicator)
{
	if (file != NULL) {
		if (file->handle != NULL && (size_t)offset < file->size) {
			/* store current position indicator */
			OFF_T position = (!change_indicator) ? FTELL(file->handle) : 0;

			if (!FSEEK(file->handle, offset, SEEK_SET)) {
				/* where the data starts on the file */
				file->offset = offset;

				/* maximum/given buffer size without end of line char */
				file->buffer.size = ((size_t)(offset + count) > file->size) ? file->size - (size_t)offset : (size_t)count;

				/* allocate memory */
				if (data_alloc(&file->buffer)) {
					/* read data from file */
					if (fread(file->buffer.address, 1, file->buffer.size, file->handle) == file->buffer.size)
						return true;
					else
						data_free(&file->buffer);
				}
				else
					data_free(&file->buffer);
			}

			/* reset file position indicator */
			if (!change_indicator)
					FSEEK(file->handle, position, SEEK_SET);
		}
	}

	return false;
}

/* read a line from file */
bool file_read_line(file_t* file, const bool change_indicator)
{
	if (file != NULL) {
		/* get current position */
		OFF_T position = FTELL(file->handle);

		/* go to end of line */
		FSCANF(file->handle, "%*s\n");

		/* count number of bytes */
		OFF_T count = FTELL(file->handle) - position;

		/* read data */
		file_read(file, position, count, change_indicator);

		/* reset file position indicator */
		if (!change_indicator)
			FSEEK(file->handle, position, SEEK_SET);

		return true;
	}

	return false;
}

/* write a block of data from buffer into file */
bool file_write(file_t* file, const OFF_T offset, const OFF_T count, const bool change_indicator)
{
	if (file != NULL) {
		if (file->handle != NULL && file->buffer.address != NULL && (size_t)offset < file->size) {
			/* store current position indicator for later */
			OFF_T position = (!change_indicator) ? FTELL(file->handle) : 0;

			if (!FSEEK(file->handle, offset, SEEK_SET)) {
				/* prevents reading data outside buffer */
				size_t size = ((size_t)count > file->buffer.size) ? file->buffer.size : (size_t)count;

				/* write data to file and return */
				if (fwrite(file->buffer.address, 1, size, file->handle) == size) {
					/* update file size */
					file->size = (size_t)file_find_size(file);

					return true;
				}
			}

			/* reset file position indicator */
			if (!change_indicator)
				FSEEK(file->handle, position, SEEK_SET);
		}
	}

	return false;
}

/* write a block of data to the file buffer */
void file_replace_buffer(file_t* file, data_t* data)
{
	if (file != NULL && data != NULL) {
		if (data->address != NULL) {
			/* resize buffer */
			void* temp_ptr = NULL;

			if ((temp_ptr = realloc(file->buffer.address, data->size)) != NULL) {
				/* store new address */
				file->buffer.address = temp_ptr;

				/* write data to file buffer */
				memcpy(file->buffer.address, data->address, data->size);

				/* update file buffer size */
				file->buffer.size = data->size;
			}
		}
	}
}

/* close file and clean up memory */
void file_close(file_t* file)
{
	if (file != NULL) {
		/* close stdio file */
		if (file->handle != NULL)
			fclose(file->handle);

		/* free memory */
		data_free(&file->buffer);
		free(file);
	}
}

/* open file without loading any data */
bool file_open(file_t** file, char* path)
{
	if (file != NULL) {
		/* initialize file struct */
		*file = calloc(sizeof(file_t), 1);

		/* asign values to struct */
		if (*file != NULL) {
			(*file)->path = path;

			if (!FOPEN(&(*file)->handle, (*file)->path, "rb+")) {
				(*file)->offset = 0;
				(*file)->size = (size_t)file_find_size(*file);

				/* initialize the data buffer */
				(*file)->buffer.size = 1;
				(*file)->buffer.address = malloc(1);

				if ((*file)->buffer.address != NULL) {
					*(*file)->buffer.address = '\0';
					return true;
				}
				else
					file_close(*file);
			}
			else
				file_close(*file);
		}
	}

	return false;
}

/* create a new file if one doesn't exist */
bool file_create(const char* filename)
{
	int file;

	/* free handle before return */
	return (OPEN(&file, filename, OFLAG, MODE)) ? false : (!CLOSE(file));
}

#ifdef __cplusplus
}
#endif

#endif /* _file_h_ */
