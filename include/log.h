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

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************
 * Outputs error to `stderr`.
 *
 * @param err error number
 * @param str string to be shown with the error,
 *            defaults to NULL
 **********************************************************/
void log_error(const int err, const char* str = NULL)
{
  if (str == NULL)
    fprintf(stderr, "%s\n", strerror(err));
  else
    fprintf(stderr, "%s %s\n", str, strerror(err));
}

/**********************************************************
 * Outputs error to `stderr` then calls exit.
 *
 * @param err error number
 * @param str string to be shown with the error
 **********************************************************/
void log_fatal(const int err, const char* str = NULL)
{
  log_error(err, str);
  exit(1);
}

#ifdef __cplusplus
}
#endif

#endif /* _log_h_ */
