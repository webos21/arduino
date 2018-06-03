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

#ifndef AVR_CFG_H
#define AVR_CFG_H

/* -----------------------------------------
 * The CFG functions for AVR
 * ----------------------------------------- */

#include "aproject.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#define FM_SIGNATURE   0x00004750L
#define FM_CFG_SECTOR  ((void *)0x0)

#define DFLAG_RPM      (0x00000001L)
#define DFLAG_SPEED    (0x00000002L)

typedef struct s_cfg_t {
	int32_t signature;
	int32_t cfg_ver;

	int32_t fdebug;
	int32_t ppk;
	int32_t rpm;
	int32_t speed;
} cfg_t;

void avr_cfg_init(void);
void avr_cfg_sync(void);
void avr_cfg_reload(void);

/*@exposed@*/
cfg_t *avr_cfg_get(void);

/*@exposed@*/
int32_t avr_cfg_is_reset(void);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* AVR_CFG_H */
