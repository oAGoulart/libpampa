/*
 * Copyright 2019 Jos� Augusto dos Santos Goulart
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
#include "image.h"

int main()
{
  image_t* img = NULL;
  if (!image_load(&img, "example.png"))
    exit(EXIT_FAILURE);

  /* do something with the image */

  image_unload(img);
  exit(EXIT_SUCCESS);
}
