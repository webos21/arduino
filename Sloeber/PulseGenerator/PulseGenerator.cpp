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

#include "Arduino.h"

#include "PulseGenerator.h"

#include "serial_adapter.h"
#include "utc_clock.h"
#include "cmd_proc.h"
#include "avr_cfg.h"

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

static main_t g_main = { false, false, false, 0, 0 };

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void gen_rpm(void);
static void gen_speed(void);
/* Forward Declaration : End */

static void gen_rpm(void) {
	if (g_main.run_rpm) {
		int32_t rps;
		int32_t tdiff;
		int32_t pcycle;

		cfg_t *cfg = avr_cfg_get();

		rps = (int32_t) (cfg->rpm / 60.0F);
		rps += ((cfg->rpm % 60) > 0) ? 1 : 0;

		/* (1000 msec / 2(up/down) / pulse per seconds) */
		pcycle = (500 / rps);

		tdiff = utc_clock_tick() - g_main.last_rpm;
		if (tdiff > pcycle) {
			if ((cfg->fdebug & DFLAG_RPM) == DFLAG_RPM) {
				serial_print(
						FC(
								"RPM> rpm = %ld / rps = %ld / pcycle = %ld / tdiff = %ld\r\n"),
						cfg->rpm, rps, pcycle, tdiff);
			}
			if (g_main.on_rpm) {
				g_main.on_rpm = false;
				digitalWrite(DPIN_RPM, LOW);
			} else {
				g_main.on_rpm = true;
				digitalWrite(DPIN_RPM, HIGH);
			}
			g_main.last_rpm = utc_clock_tick();
		}
	}
}

static void gen_speed(void) {
	if (g_main.run_speed) {
		int32_t sps;
		int32_t tdiff;
		int32_t pcycle;

		cfg_t *cfg = avr_cfg_get();

		/* PPK / 3600 = 1km pulse for seconds (+0.5F round) */
		sps = (int32_t) ((cfg->speed * (cfg->ppk / 3600.0F)) + 0.5F);

		/* (1000 msec / 2(up/down) / pulse per seconds) */
		pcycle = (500 / sps);

		tdiff = utc_clock_tick() - g_main.last_speed;
		if (tdiff > pcycle) {
			if ((cfg->fdebug & DFLAG_SPEED) == DFLAG_SPEED) {
				serial_print(
						FC(
								"SPEED> speed = %ld / sps = %ld / pcycle = %ld / tdiff = %ld\r\n"),
						cfg->speed, sps, pcycle, tdiff);
			}
			if (g_main.on_speed) {
				g_main.on_speed = false;
				digitalWrite(DPIN_SPEED, LOW);
			} else {
				g_main.on_speed = true;
				digitalWrite(DPIN_SPEED, HIGH);
			}
			g_main.last_speed = utc_clock_tick();
		}
	}
}

/* ----------------------------------------------
 * ARDUINO Functions
 * ---------------------------------------------- */

void setup() {
	pinMode(DPIN_RPM, OUTPUT);
	pinMode(DPIN_SPEED, OUTPUT);

	Serial.begin(9600);

	avr_cfg_init();
	utc_clock_init();

	cmd_proc_show_summary();
	Serial.print(F("PG> "));
}

void loop() {
	gen_rpm();
	gen_speed();

	serial_process();
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void gen_start_rpm(void) {
	g_main.run_rpm = true;
}

void gen_stop_rpm(void) {
	g_main.run_rpm = false;
	digitalWrite(DPIN_RPM, LOW);
}

void gen_start_speed(void) {
	g_main.run_speed = true;
}

void gen_stop_speed(void) {
	g_main.run_speed = false;
	digitalWrite(DPIN_SPEED, LOW);
}

