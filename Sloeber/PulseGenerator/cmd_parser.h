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
#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "aproject.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CMD_ITM_LEN
#define CMD_ITM_LEN   128  /* queue item length        */
#endif /* CMD_ITM_LEN */

#define CMD_PARAM_MAX 16

typedef enum e_cmd_e {
	CMD_UNKNOWN,
	CMD_HELP,
	CMD_SHOW,
	CMD_CFG,
	CMD_START,
	CMD_STOP,
	CMD_TEST,
	CMD_REBOOT
} cmd_e;

typedef struct s_cmd_t {
	/*@unique@*/
	char_t line[CMD_ITM_LEN];
	cmd_e cno;
	/*@dependent@*/
	char_t *cmd;
	int32_t nparam;
	char_t *param[CMD_PARAM_MAX];
} cmd_t;

int32_t /*@alt void@*/svc_cmd_parse(cmd_t *ct, char_t *cmd, uint32_t cmdlen);
void svc_cmd_print(const cmd_t *ct);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* CMD_PARSER_H */
