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

#include "cmd_parser.h"

#include "serial_adapter.h"

#include <string.h>

#ifdef __ICCARM__
#pragma diag_suppress=Pm073,Pm144
#endif

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static void svc_cmd_enum(cmd_t *ct);
/* Forward Declaration : End */

static void svc_cmd_enum(cmd_t *ct) {
	if (strcmp("cfg", ct->cmd) == 0) {
		ct->cno = CMD_CFG;
	} else if (strcmp("start", ct->cmd) == 0) {
		ct->cno = CMD_START;
	} else if (strcmp("stop", ct->cmd) == 0) {
		ct->cno = CMD_STOP;
	} else if (strcmp("test", ct->cmd) == 0) {
		ct->cno = CMD_TEST;
	} else if (strcmp("reboot", ct->cmd) == 0) {
		ct->cno = CMD_REBOOT;
	} else if (strcmp("help", ct->cmd) == 0) {
		ct->cno = CMD_HELP;
	} else if (strcmp("show", ct->cmd) == 0) {
		ct->cno = CMD_SHOW;
	} else {
		ct->cno = CMD_UNKNOWN;
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t svc_cmd_parse(cmd_t *ct, char_t *cmd, uint32_t cmdlen) {
	char_t *pos;
	uint32_t index;
	uint8_t pc;

	ct->cno = CMD_UNKNOWN;
	ct->cmd = ct->line;
	ct->nparam = 0;
	for (pc = 0; pc < CMD_PARAM_MAX; pc++) {
		ct->param[pc] = NULL;
	}

	memset(ct->line, 0, sizeof(ct->line));
	if (cmdlen >= sizeof(ct->line)) {
		cmdlen = sizeof(ct->line) - 1;
	}
	memcpy(ct->line, cmd, cmdlen);

	pos = ct->line;
	index = 0;
	pc = 0;
	while ((0 != pos[index]) && (index < cmdlen)) {
		/* skip white space */
		for (;;) {
			if ((0 == pos[index]) || (index >= cmdlen)) {
				/*@innerbreak@*/
				break;
			}

			if ((' ' == pos[index]) || ('\t' == pos[index])
					|| ('\r' == pos[index]) || ('\n' == pos[index])) {
				index++;
			} else {
				/*@innerbreak@*/
				break;
			}
		}

		if ((0 == pos[index]) || (index >= cmdlen)) {
			break;
		}

		if (pc < CMD_PARAM_MAX) {
			ct->param[pc] = &pos[index];
			ct->nparam++;
			pc++;
		} else {
			break;
		}

		/* search for white space */
		for (;;) {
			if ((0 == pos[index]) || (index >= cmdlen)) {
				/*@innerbreak@*/
				break;
			}

			if ((' ' == pos[index]) || ('\t' == pos[index])
					|| ('\r' == pos[index]) || ('\n' == pos[index])) {
				/*@innerbreak@*/
				break;
			} else {
				index++;
			}
		}

		/* spilit into tokens */
		if ((0 == pos[index]) || (index >= cmdlen)) {
			break;
		} else {
			pos[index] = 0;
			index++;
		}
	} /* while */

	if (ct->nparam > 0) {
		ct->cmd = ct->param[0];
	}

	svc_cmd_enum(ct);

	return (ct->nparam);
}

void svc_cmd_print(const cmd_t *ct) {
	serial_print(FC("COMMAND: cmd  = %s / np = %d\r\n"), ct->cmd, ct->nparam);
	serial_print(FC("         p[00] = %s / p[01] = %s\r\n"), ct->param[0],
			ct->param[1]);
	serial_print(FC("         p[02] = %s / p[03] = %s\r\n"), ct->param[2],
			ct->param[3]);
	serial_print(FC("         p[04] = %s / p[05] = %s\r\n"), ct->param[4],
			ct->param[5]);
	serial_print(FC("         p[06] = %s / p[07] = %s\r\n"), ct->param[6],
			ct->param[7]);
	serial_print(FC("         p[08] = %s / p[09] = %s\r\n"), ct->param[8],
			ct->param[9]);
	serial_print(FC("         p[10] = %s / p[11] = %s\r\n"), ct->param[10],
			ct->param[11]);
	serial_print(FC("         p[12] = %s / p[13] = %s\r\n"), ct->param[12],
			ct->param[13]);
	serial_print(FC("         p[14] = %s / p[15] = %s\r\n"), ct->param[14],
			ct->param[15]);
}
