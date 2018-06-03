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

/* ----------------------------------------------
 * Headers
 * ---------------------------------------------- */

#include "avr_cfg.h"

#include <avr/eeprom.h>

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

static cfg_t g_cfg;
static int32_t g_cfg_reset = 0;

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void avr_cfg_reset(void);
/* Forward Declaration : End */

static void avr_cfg_reset(void) {
	memset(&g_cfg, 0x0, sizeof(cfg_t));

	g_cfg.signature = FM_SIGNATURE;
	g_cfg.cfg_ver = sizeof(cfg_t);

	g_cfg.fdebug = 0;
	g_cfg.ppk = 4000;
	g_cfg.rpm = 120;
	g_cfg.speed = 60;

	avr_cfg_sync();
	g_cfg_reset = 1;
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void avr_cfg_init(void) {
	memset(&g_cfg, 0x0, sizeof(cfg_t));
	avr_cfg_reload();
}

void avr_cfg_sync(void) {
	eeprom_write_block(&g_cfg, FM_CFG_SECTOR, sizeof(cfg_t));
}

void avr_cfg_reload(void) {
	eeprom_read_block(&g_cfg, FM_CFG_SECTOR, sizeof(cfg_t));
	if ((g_cfg.signature != FM_SIGNATURE)
			|| (g_cfg.cfg_ver != (int32_t) sizeof(cfg_t))) {
		avr_cfg_reset();
	}
}

cfg_t *avr_cfg_get(void) {
	return &g_cfg;
}

int32_t avr_cfg_is_reset(void) {
	return g_cfg_reset;
}
