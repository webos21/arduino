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

#include "cmd_proc.h"

#include "PulseGenerator.h"
#include "cmd_parser.h"
#include "serial_adapter.h"
#include "utc_clock.h"
#include "avr_cpu.h"
#include "avr_cfg.h"

#include <string.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

static cmd_t g_cmd; /* for reducing the stack-size */
static char_t g_tmpbuf[64]; /* for reducing the stack-size */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void cmd_proc_line_b(void);
static void cmd_proc_line_c(void);
static void cmd_proc_line_e(void);
static void cmd_proc_show_mcu(void);
static void cmd_proc_show_cfg(void);
static void cmd_proc_show_all(void);
static int32_t /*@alt void@*/cmd_proc_show(char_t *cmd, int32_t cmdlen,
		const cmd_t *chnd);
static int32_t /*@alt void@*/cmd_proc_cfg(char_t *cmd, int32_t cmdlen,
		const cmd_t *chnd);
static int32_t /*@alt void@*/cmd_proc_test(char_t *cmd, int32_t cmdlen,
		const cmd_t *chnd);
/* Forward Declaration : End */

static void cmd_proc_line_b(void) {
	serial_print(FC("\r\n=========================================\r\n"));
}

static void cmd_proc_line_c(void) {
	serial_print(FC("-----------------------------------------\r\n"));
}

static void cmd_proc_line_e(void) {
	serial_print(FC("=========================================\r\n\r\n"));
}

static void cmd_proc_show_mcu(void) {
	uint8_t cpuId[3] = { 0U, 0U, 0U };
	uint16_t flashSize = 0;

	avr_cpu_id(cpuId);
	flashSize = avr_cpu_flash_size();

	serial_print(FC("[MCU]\r\n"));
	serial_print(FC("  * ID            : %02X %02X %02X\r\n"), cpuId[0],
			cpuId[1], cpuId[2]);
#if 0 /* reference code for (serial_hexdump2) */
	serial_hexdump2(cpuId, sizeof(cpuId), 0);
#endif
	serial_print(FC("  * Flash Size    : %d KB\r\n"), flashSize);
}

static void cmd_proc_show_cfg(void) {
	cfg_t *cfg = avr_cfg_get();

	memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
	memcpy(g_tmpbuf, &cfg->signature, sizeof(cfg->signature));

	serial_print(FC("[CFG]\r\n"));
	/* print-sequence is important : i don't know why?? */
	serial_print(FC("  * Signature  : %s (0x%08X)\r\n"), g_tmpbuf,
			cfg->signature);
	serial_print(FC("  * Version    : %d\r\n"), cfg->cfg_ver);
	serial_print(FC("  * IsReset?   : %d\r\n"), avr_cfg_is_reset());
	serial_print(FC("  * DEBUG_FLAG : 0x%08X\r\n"), cfg->fdebug);
	serial_print(FC("  * PPK        : %d\r\n"), cfg->ppk);
	serial_print(FC("  * RPM        : %d rpm\r\n"), cfg->rpm);
	serial_print(FC("  * SPEED      : %d km\r\n"), cfg->speed);
}

static void cmd_proc_show_all(void) {
	cmd_proc_line_b();
	cmd_proc_show_mcu();
	cmd_proc_line_c();
	cmd_proc_show_cfg();
	cmd_proc_line_e();
}

static int32_t cmd_proc_show(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = 0L;

	if (chnd->nparam < 2) {
		cmd_proc_show_summary();
	} else {
		if (strcmp("all", chnd->param[1]) == 0) {
			cmd_proc_show_all();
		} else if (strcmp("mcu", chnd->param[1]) == 0) {
			cmd_proc_line_b();
			cmd_proc_show_mcu();
			cmd_proc_line_e();
		} else if (strcmp("cfg", chnd->param[1]) == 0) {
			cmd_proc_line_b();
			cmd_proc_show_cfg();
			cmd_proc_line_e();
		} else {
			serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
					cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t cmd_proc_cfg(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 3) {
		serial_print(FC("Invalid arguments(%d < 3): %s\r\n"), chnd->nparam,
				cmd);
	} else {
		rv = 0L;
		if (strcmp("rpm", chnd->param[1]) == 0) {
			cfg_t *cfg = avr_cfg_get();
			int32_t rpm = strtol(chnd->param[2], NULL, 10);
			if (rpm < 120) {
				serial_print(FC("Invalid RPM : %d\r\n"), rpm);
			} else {
				cfg->rpm = rpm;
				avr_cfg_sync();
				serial_print(FC("RPM set to %d\r\n"), rpm);
			}
		} else if (strcmp("ppk", chnd->param[1]) == 0) {
			cfg_t *cfg = avr_cfg_get();
			cfg->ppk = strtol(chnd->param[2], NULL, 10);
			avr_cfg_sync();
			serial_print(FC("PPK set to %d\r\n"), cfg->ppk);
		} else if (strcmp("speed", chnd->param[1]) == 0) {
			cfg_t *cfg = avr_cfg_get();
			cfg->speed = strtol(chnd->param[2], NULL, 10);
			avr_cfg_sync();
			serial_print(FC("SPEED set to %d\r\n"), cfg->speed);
		} else if (strcmp("debug", chnd->param[1]) == 0) {
			if (strcmp("rpm_on", chnd->param[2]) == 0) {
				cfg_t *cfg = avr_cfg_get();
				cfg->fdebug |= DFLAG_RPM;
				avr_cfg_sync();
				serial_print(FC("RPM DEBUG ON (debug_flag = 0x%08X\r\n"),
						cfg->fdebug);
			} else if (strcmp("rpm_off", chnd->param[2]) == 0) {
				cfg_t *cfg = avr_cfg_get();
				cfg->fdebug = (cfg->fdebug & (~DFLAG_RPM));
				avr_cfg_sync();
				serial_print(FC("RPM DEBUG OFF (debug_flag = 0x%08X\r\n"),
						cfg->fdebug);
			} else if (strcmp("speed_on", chnd->param[2]) == 0) {
				cfg_t *cfg = avr_cfg_get();
				cfg->fdebug |= DFLAG_SPEED;
				avr_cfg_sync();
				serial_print(FC("SPEED DEBUG ON (debug_flag = 0x%08X\r\n"),
						cfg->fdebug);
			} else if (strcmp("speed_off", chnd->param[2]) == 0) {
				cfg_t *cfg = avr_cfg_get();
				cfg->fdebug = (cfg->fdebug & (~DFLAG_SPEED));
				avr_cfg_sync();
				serial_print(FC("SPEED DEBUG OFF (debug_flag = 0x%08X\r\n"),
						cfg->fdebug);
			} else {
				/* Nothing to do */
			}
		} else {
			serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
					cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

static int32_t cmd_proc_test(char_t *cmd, int32_t cmdlen, const cmd_t *chnd) {
	int32_t rv = -1L;

	if (chnd->nparam < 2) {
		serial_print(FC("Invalid arguments(%d < 3): %s\r\n"), chnd->nparam,
				cmd);
	} else {
		rv = 0L;

		if (strcmp("rpm_on", chnd->param[1]) == 0) {
			digitalWrite(DPIN_RPM, HIGH);
		} else if (strcmp("rpm_off", chnd->param[1]) == 0) {
			digitalWrite(DPIN_RPM, LOW);
		} else if (strcmp("speed_on", chnd->param[1]) == 0) {
			digitalWrite(DPIN_SPEED, HIGH);
		} else if (strcmp("speed_off", chnd->param[1]) == 0) {
			digitalWrite(DPIN_SPEED, LOW);
		} else if (strcmp("usec", chnd->param[1]) == 0) {
			if (chnd->nparam < 3) {
				serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
						cmdlen);
				rv = -1L;
			} else {
				int32_t wt;
				int64_t stick;
				int64_t etick;
				int32_t dtick;

				wt = strtol(chnd->param[2], NULL, 10);

				stick = utc_clock_tick();
				delayMicroseconds(wt);
				etick = utc_clock_tick();

				dtick = etick - stick;
				serial_print(FC("%d usec = %d\r\n"), wt, dtick);
			}
		} else if (strcmp("msec", chnd->param[1]) == 0) {
			if (chnd->nparam < 3) {
				serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
						cmdlen);
				rv = -1L;
			} else {
				int32_t wt;
				int64_t stick;
				int64_t etick;
				int32_t dtick;

				wt = strtol(chnd->param[2], NULL, 10);

				stick = utc_clock_tick();
				delay(wt);
				etick = utc_clock_tick();

				dtick = etick - stick;
				serial_print(FC("%d msec = %d\r\n"), wt, dtick);
			}
		} else {
			serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
					cmdlen);
			rv = -1L;
		}
	}

	return rv;
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void cmd_proc_help(void) {
	cmd_proc_line_b();
	serial_print(FC("* Usages)\r\n"));
	serial_print(FC("  - help         : show the commands (this usages)\r\n"));
	serial_print(FC("  - show         : show the system summary\r\n"));
	serial_print(FC("  - show all     : show the full status\r\n"));
	serial_print(FC("  - show mcu     : show the MCU status\r\n"));
	serial_print(FC("  - show cfg     : show the CONFIGURATION\r\n"));
	serial_print(FC("  - cfg rpm 60   : cfg RPM to 60\r\n"));
	serial_print(FC("  - cfg speed 60 : cfg SPEED to 60\r\n"));
	serial_print(FC("  - test [opts.] : test commands\r\n"));
	serial_print(FC("  - reboot       : reboot the ARDUINO\r\n"));
	cmd_proc_line_e();
}

void cmd_proc_show_summary(void) {
	utc_ts_t uts;

	utc_clock_ts(&uts);
	memset(g_tmpbuf, 0, sizeof(g_tmpbuf));
	(void) snprintf_P(g_tmpbuf, sizeof(g_tmpbuf),
			FC("%02d-%02d-%02d %02d:%02d:%02d"), uts.year, uts.mon, uts.day,
			uts.hour, uts.min, uts.sec);

	cmd_proc_line_b();
	serial_print(FC("[Pulse Generator]\r\n"));
	serial_print(FC("  * Build by     : AVR\r\n"));
	serial_print(FC("  * Up-Time      : %d (sec)\r\n"), utc_clock_tick_sec());
	serial_print(FC("  * Current Time : %s\r\n"), g_tmpbuf);
	cmd_proc_line_e();
}

void cmd_proc(char_t *cmd, int32_t cmdlen) {
	if (cmdlen > 2) {
		svc_cmd_parse(&g_cmd, cmd, cmdlen);

		switch (g_cmd.cno) {
		case CMD_SHOW:
			cmd_proc_show(cmd, cmdlen, &g_cmd);
			break;
		case CMD_HELP:
			cmd_proc_help();
			break;
		case CMD_CFG:
			cmd_proc_cfg(cmd, cmdlen, &g_cmd);
			break;
		case CMD_START:
			if (strcmp("rpm", g_cmd.param[1]) == 0) {
				gen_start_rpm();
			} else if (strcmp("speed", g_cmd.param[1]) == 0) {
				gen_start_speed();
			} else {
				serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
						cmdlen);
			}
			break;
		case CMD_STOP:
			if (strcmp("rpm", g_cmd.param[1]) == 0) {
				gen_stop_rpm();
			} else if (strcmp("speed", g_cmd.param[1]) == 0) {
				gen_stop_speed();
			} else {
				serial_print(FC("Invalid arguments : %s (len=%d)\r\n"), cmd,
						cmdlen);
			}
			break;
		case CMD_TEST:
			cmd_proc_test(cmd, cmdlen, &g_cmd);
			break;
		case CMD_REBOOT:
			avr_cpu_reboot();
			break;
		default:
			serial_hexdump(const_cast<char *>("Unknown command"), cmd, cmdlen);
			break;
		}
		serial_print(FC("PG> "));
	} else {
		serial_print(FC("\r\nPG> "));
	}
}
