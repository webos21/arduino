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
#ifndef SERIAL_ADAPTER_H
#define SERIAL_ADAPTER_H

#include "aproject.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define LOG_LINE_MAX    128 /* WARN : "serial_print" print the maximum 128 characters */

int32_t /*@alt void@*/serial_print(const char_t *fmt, ...);

void serial_hexdump(char_t *desc, void *addr, int32_t len);
void serial_hexdump2(void *addr, int32_t len, int32_t offset);

void serial_process(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* SERIAL_ADAPTER_H */
