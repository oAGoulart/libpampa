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

#ifndef _types_h_
#define _types_h_

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined (__MINGW64__) || defined(__CYGWIN__)
  #undef __WINDOWS__
  #define __WINDOWS__ 1
#endif

#if defined(__ILP32__) || defined(_ILP32) || defined(__i386__) || defined(_M_IX86) || defined(_X86_)
  #undef __X86_ARCH__
  #define __X86_ARCH__ 1
#endif

/** unsigned byte data type */
typedef unsigned char ubyte_t;

#ifdef __X86_ARCH__
  /** unsigned long integer data type */
  typedef uint32_t ulong_t;
#else
  /** unsigned long integer data type */
  typedef uint64_t ulong_t;
#endif

#endif /* _types_h_ */
