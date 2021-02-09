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

#ifndef _log_h_
#define _log_h_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifndef NDEBUG
/**********************************************************
 * Outputs error to stderr.
 *
 * @param str string to be shown
 **********************************************************/
#define LOG(str) \
  fprintf(stderr, "%s:%i %s\n", __FILE__, __LINE__, str)

/**********************************************************
 * Logs then calls exit.
 *
 * @param str string to be shown
 **********************************************************/
#define FATAL(str) \
  LOG(str) \
  exit(EXIT_FAILURE)
#endif

#endif /* _log_h_ */
