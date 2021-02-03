/*
 * Copyright 2019-2021 José Augusto dos Santos Goulart
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

  #define MODE   _S_IREAD | _S_IWRITE
  #define OFLAG  _O_RDWR | _O_CREAT | _O_EXCL
  #define CLOSE  _close
  #define FOPEN  fopen_s
  #define FSCANF fscanf_s

  #define OPEN(handle, filename, flag, mode) (_sopen_s(handle, filename, flag, _SH_DENYNO, mode))
#else /* assume POSIX */
  #include <unistd.h>

  #ifdef X64_FILES
    #define FTELL ftello
    #define FSEEK fseeko
  #endif

  #define STAT   stat /* always stat64 on Linux kernel 2.4+ */
  #define MODE   0777
  #define OFLAG  O_RDWR | O_CREAT | O_EXCL
  #define CLOSE  close
  #define FSCANF fscanf

  #define OPEN(handle, filename, flag, mode) (((*handle = open(filename, flag, mode)) == -1) ? errno : false)
  #define FOPEN(file, filename, mode) (((*file = fopen(filename, mode)) == NULL) ? errno : false)
#endif

/**
 * Data used to work with files.
 */
typedef struct file_s
{
  /** file path */
  char*  path;
  /** stdio file handle */
  FILE*  handle;
  /** where buffer data starts */
  OFF_T  offset;
  /** size of the file in disk */
  size_t size;
  /** buffer to load file data */
  data_t buffer;
} file_t;

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************
 * Find the system storage size of a file
 *
 * @param file reference to file struct
 *
 * @return OFF_T file size
 **********************************************************/
OFF_T file_find_size(const file_t* file)
{
  if (file != NULL) {
    struct STAT file_status;

    if (!STAT(file->path, &file_status))
      return file_status.st_size;
  }

  return 0;
}

/**********************************************************
 * Read a block of data from system storage and store it into
 * the file buffer.
 *
 * @param file reference to file struct
 * @param offset position from start of file
 * @param count amount of bytes to be read
 * @param change_indicator should file cursor stay changed
 *                         after reading from file?
 *
 * @return bool true if successful, false if failed
 **********************************************************/
bool file_read(file_t* file, const OFF_T offset, const OFF_T count, const bool change_indicator)
{
  if (file != NULL) {
    if (file->handle != NULL && (size_t)offset < file->size) {
      OFF_T old_pos = (!change_indicator) ? FTELL(file->handle) : 0;

      if (!FSEEK(file->handle, offset, SEEK_SET)) {
        file->offset = offset;

        /* maximum or given buffer size without end of line char */
        file->buffer.size = ((size_t)(offset + count) > file->size) ? file->size - (size_t)offset : (size_t)count;

        if (data_alloc(&file->buffer)) {
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
        FSEEK(file->handle, old_pos, SEEK_SET);
    }
  }

  return false;
}

/**********************************************************
 * Read a line from file.
 *
 * @param file reference to file struct
 * @param change_indicator should file cursor stay changed
 *                         after reading from file?
 *
 * @return bool true if successful, false if failed
 **********************************************************/
bool file_read_line(file_t* file, const bool change_indicator)
{
  if (file != NULL) {
    OFF_T position = FTELL(file->handle);

    /* go to end of line */
    FSCANF(file->handle, "%*s\n");

    /* count number of bytes */
    OFF_T count = FTELL(file->handle) - position;

    file_read(file, position, count, change_indicator);

    if (!change_indicator)
      FSEEK(file->handle, position, SEEK_SET);

    return true;
  }

  return false;
}

/**********************************************************
 * Flush block of data from file buffer into system storage.
 *
 * @param file reference to file struct
 * @param offset position from start of file
 * @param count amount of bytes to be write
 * @param change_indicator should file cursor stay changed
 *                         after writing to file?
 *
 * @return bool true if successful, false if failed
 **********************************************************/
bool file_write(file_t* file, const OFF_T offset, const OFF_T count, const bool change_indicator)
{
  if (file != NULL) {
    if (file->handle != NULL && file->buffer.address != NULL && (size_t)offset < file->size) {
      OFF_T old_pos = (!change_indicator) ? FTELL(file->handle) : 0;

      if (!FSEEK(file->handle, offset, SEEK_SET)) {
        /* prevents reading data outside buffer */
        size_t size = ((size_t)count > file->buffer.size) ? file->buffer.size : (size_t)count;

        if (fwrite(file->buffer.address, 1, size, file->handle) == size) {
          file->size = (size_t)file_find_size(file);

          return true;
        }
      }

      /* reset file position indicator */
      if (!change_indicator)
        FSEEK(file->handle, old_pos, SEEK_SET);
    }
  }

  return false;
}

/**********************************************************
 * Write block of data into file buffer. The file buffer will
 * be replaced.
 *
 * @param file reference to file struct
 * @param data block of data to replace file buffer
 **********************************************************/
void file_replace_buffer(file_t* file, data_t* data)
{
  if (file != NULL && data != NULL) {
    if (data->address != NULL) {
      void* temp_ptr = NULL;

      if ((temp_ptr = realloc(file->buffer.address, data->size)) != NULL) {
        file->buffer.address = temp_ptr;

        memcpy(file->buffer.address, data->address, data->size);

        file->buffer.size = data->size;
      }
    }
  }
}

/**********************************************************
 * Close open file and clean up memory.
 *
 * @param file reference to file struct
 **********************************************************/
void file_close(file_t* file)
{
  if (file != NULL) {
    if (file->handle != NULL)
      fclose(file->handle);

    data_free(&file->buffer);
    free(file);
  }
}

/**********************************************************
 * Open a file and initialize struct values. The file should
 * exist.
 *
 * @param file reference to pointer where file struct is located
 * @param path string to file path
 *
 * @return bool true if successful, false if failed
 **********************************************************/
bool file_open(file_t** file, char* path)
{
  if (file != NULL) {
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

/**********************************************************
 * Create a new file. The new file should not exist.
 * Also, the file should be at an existing directory.
 *
 * @param filename relative or absolute path to the new file
 *
 * @return bool true if successful, false if failed
 **********************************************************/
bool file_create(const char* filename)
{
  int file;

  /* free handle before return */
  return (OPEN(&file, filename, OFLAG, MODE)) ? false : (!(CLOSE(file)));
}

#ifdef __cplusplus
}
#endif

#endif /* _file_h_ */
