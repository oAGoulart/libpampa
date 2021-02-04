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

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************
 * Verify if given file is a PNG image.
 *
 * @param file pointer loaded image file
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

#ifdef __cplusplus
}
#endif

#endif /* _image_h_ */
