/*
 * Copyright 2018 Cheolmin Jo (webos21@gmail.com)
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

#ifndef APROJCT_H
#define APROJCT_H

/* -----------------------------------------
 * Type Definitions for PROJECT
 * (include MISRA C 2004)
 * ----------------------------------------- */

/*
 * ARDUINO MACRO
 */
#ifdef __cplusplus
#define FC(x)  (reinterpret_cast<const char *>(F(x)))
#define FN(x)  (const_cast<char*>(reinterpret_cast<const char *>(F(x))))
#else
#define FC(x)  x
#define FN(x)  x
#endif

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NOPT
#define NOPT   volatile
#endif /* NOPT */

#if defined(_WIN32) && !defined(__MINGW32__) && !defined(__GNUC__) && \
  (!defined(_MSC_VER) || _MSC_VER<1600) && !defined(__WINE__)
#include <BaseTsd.h>
#include <stddef.h>
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <inttypes.h>
#endif

typedef char char_t;

typedef float float32_t;
typedef double float64_t;

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* APROJCT_H */
