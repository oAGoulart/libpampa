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

#ifndef _image_h_
#define _image_h_

#include "file.h"

/**
 * Data used to work with images.
 */
typedef struct image_s
{
  /** image width */
  size_t  width;
  /** image height */
  size_t  height;
  /** depth of each pixel */
  ubyte_t depth;
  /** type of color used
   *  represent sums of the following values:
   *    1 (palette used)
   *    2 (color used)
   *    4 (alpha channel used)
   *  valid values are 0, 2, 3, 4, and 6 */
  ubyte_t color;
  /** compression method
   *  must be 0 (zlib deflate/inflate) */
  ubyte_t compression;
  /** image filter
   *  must be 0 */
  ubyte_t filter;
  /** transmission order of data
   *  must be either:
   *    0 (no interlace)
   *    1 (Adam7 interlace) */
  ubyte_t interlace;
  /** pixel buffer */
  data_t buffer;
} image_t;

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************
 * Verify if given file is a PNG image.
 *
 * @param file pointer to loaded image file
 *
 * @return bool whether or not file is a PNG image
 **********************************************************/
bool is_png(file_t* file)
{
  if (file != NULL) {
    if (file_read(file, 0, 8, false))
      return !memcmp("\211PNG\r\n\032\n", file->buffer.address, 8);
  }

  return false;
}

/**********************************************************
 * Free image from memory.
 *
 * @param file pointer to loaded image file
 **********************************************************/
void image_unload(image_t* img)
{
  if (img != NULL) {
    data_free(&img->buffer);
    free(img);
  }
}

/**********************************************************
 * Load image into memory as it is (filtered/interlaced).
 *
 * @param file pointer to empty image pointer
 *
 * @return bool whether or not file was loaded
 **********************************************************/
bool image_load(image_t** image, char* path)
{
  bool successful = false;

  if (path != NULL && image != NULL) {
    file_t* img = NULL;
    if (!file_open(&img, path))
      return successful;

    *image = (image_t*)calloc(sizeof(image_t), 1);
    if (*image == NULL) {
      /* TODO: Add other types of raster images */
      if (is_png(img)) {
        if (file_seek(img, 8)) {
          size_t chunks = 0;
          while (file_read(img, 0, 8, true)) {
            chunks++;
            uint32_t length = *(uint32_t*)img->buffer.address;
            ubyte_t* type = img->buffer.address + 4;

            if (!memcmp("IHDR", type, 4)) {
              if (chunks != 1 || length != 13 || !file_read(img, 0, length, true))
                break;

              (*image)->width = (size_t)*(int32_t*)img->buffer.address;
              (*image)->height = (size_t)*(int32_t*)img->buffer.address + 4;
              (*image)->depth = (ubyte_t)*(int8_t*)img->buffer.address + 8;
              (*image)->color = (ubyte_t)*(int8_t*)img->buffer.address + 9;
              (*image)->compression = (ubyte_t)*(int8_t*)img->buffer.address + 10;
              (*image)->filter = (ubyte_t)*(int8_t*)img->buffer.address + 11;
              (*image)->interlace = (ubyte_t)*(int8_t*)img->buffer.address + 12;

              if ((*image)->compression || (*image)->filter || (*image)->interlace > 1)
                LOG("Warning: PNG format has invalid values.");
            }

            if (!memcmp("IDAT", type, 4)) {
              if (chunks == 1 || !file_read(img, 0, length, true))
                break;

              if (!data_append(&img->buffer, &(*image)->buffer))
                LOG("Warning: Could not properly load image, output may be corrupted.");
            }

            if (!memcmp("IEND", type, 4)) {
              if (chunks != 1)
                successful = true;
              break;
            }

            (void)file_seek(img, img->offset + 4);
          }

          /* deflate image */
          if (successful) {
            ~successful;
            data_t deflated = { (*image)->buffer.size, NULL };
            if (data_alloc(&deflated)) {
              if (zlib_deflate(&(*image)->buffer, &deflated)){
                if (data_copy(&deflated, &(*image)->buffer))
                 ~successful;
              }
            }
            data_free(&deflated);
          }
        }
      }
    }
    file_close(img);
  }

  if (!successful)
    image_unload(*image);
  return successful;
}

#ifdef __cplusplus
}
#endif

#endif /* _image_h_ */
