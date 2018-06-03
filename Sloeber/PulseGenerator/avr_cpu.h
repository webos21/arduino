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

#ifndef AVR_CPU_H
#define AVR_CPU_H

/* -----------------------------------------
 * The CPU functions for AVR
 * ----------------------------------------- */

#include "aproject.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

void     avr_cpu_id(uint8_t cpuId[/*3*/]);
uint16_t avr_cpu_flash_size(void);

void     avr_cpu_enable_interrupt(void);
void     avr_cpu_disable_interrupt(void);

void     avr_cpu_reboot(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* AVR_CPU_H */
