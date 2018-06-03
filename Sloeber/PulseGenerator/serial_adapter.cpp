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

#include "serial_adapter.h"

#include "cmd_proc.h"

#include <string.h>
#include <stdarg.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#define SERIAL_RX_BUF_SIZE   128

static char_t g_rbuf[SERIAL_RX_BUF_SIZE];
static int32_t g_rcnt = 0;
static int32_t g_line_ok = 0;

static char_t g_msg[LOG_LINE_MAX]; /* for reducing the stack-size */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

int32_t serial_print(const char_t *fmt, ...) {
	va_list va;

	/* Boost Variables */
	int32_t msglen = 0;

	memset(g_msg, 0, sizeof(g_msg));

	va_start(va, fmt);
#ifdef __AVR__
	msglen = vsnprintf_P(g_msg, (sizeof(g_msg) - 1U), fmt, va); /* progmem for AVR */
#else
	msglen = vsnprintf(g_msg, (sizeof(g_msg) - 1U), fmt, va); /* for the rest of the world */
#endif
	va_end(va);

	Serial.print(g_msg);

	return msglen;
}

void serial_hexdump(char_t *desc, void *addr, int32_t len) {
	int32_t i;
	uint8_t buff[32];
	uint8_t *pc = (uint8_t *) addr;

	/* Output description if given. */
	if (desc != NULL) {
		serial_print(FC("%s:\r\n"), desc);
	}

	if (len > 0L) {
		/* Process every byte in the data. */
		for (i = 0; i < len; i++) {
			/* Multiple of 16 means new line (with line offset). */

			if ((i % 16) == 0) {
				/* Just don't print ASCII for the zeroth line. */
				if (i != 0) {
					serial_print(FC("  %s\r\n"), buff);
				}

				/* Output the offset. */
				serial_print(FC("  %06x "), i);
			}

			/* Now the hex code for the specific character. */
			serial_print(FC(" %02x"), pc[i]);

			/* And store a printable ASCII character for later. */
			if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
				buff[i % 16] = '.';
			} else {
				buff[i % 16] = pc[i];
			}
			buff[(i % 16) + 1] = '\0';
		}

		/* Pad out last line if not exactly 16 characters. */
		while ((i % 16) != 0L) {
			serial_print(FC("   "));
			i++;
		}

		/* And print the final ASCII bit. */
		serial_print(FC("  %s\r\n"), buff);
	} else if (len == 0L) {
		serial_print(FC("  ZERO LENGTH\r\n"));
	} else {
		serial_print(FC("  NEGATIVE LENGTH: %i\r\n"), len);
	}
}

void serial_hexdump2(void *addr, int32_t len, int32_t offset) {
	int32_t i;
	uint8_t buff[32];
	uint8_t *pc = (uint8_t *) addr;

	if (len > 0L) {
		/* Process every byte in the data. */
		for (i = 0; i < len; i++) {
			/* Multiple of 16 means new line (with line offset). */

			if ((i % 16) == 0) {
				/* Just don't print ASCII for the zeroth line. */
				if (i != 0) {
					serial_print(FC("  %s\r\n"), buff);
				}

				/* Output the offset. */
				serial_print(FC("  %06x "), (i + offset));
			}

			/* Now the hex code for the specific character. */
			serial_print(FC(" %02x"), pc[i]);

			/* And store a printable ASCII character for later. */
			/* 0x20 == ' ' && 0x7e == '~' */
			if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
				buff[i % 16] = '.';
			} else {
				buff[i % 16] = pc[i];
			}
			buff[(i % 16) + 1] = '\0';
		}

		/* Pad out last line if not exactly 16 characters. */
		while ((i % 16) != 0) {
			serial_print(FC("   "));
			i++;
		}

		/* And print the final ASCII bit. */
		serial_print(FC("  %s\r\n"), buff);
	} else if (len == 0) {
		serial_print(FC("  ZERO LENGTH\r\n"));
	} else {
		serial_print(FC("  NEGATIVE LENGTH: %i\r\n"), len);
	}
}

void serial_process(void) {
	if (g_line_ok) {
		cmd_proc(g_rbuf, g_rcnt);
		g_rcnt = 0;
		g_line_ok = 0;
	}
}

/* ------------------------------------------------
 * The SERIAL interrupt handler of ARDUINO
 * ------------------------------------------------ */

void serialEvent() {
	int rc = Serial.read();
	if ((rc > 0) && (rc != '\n')) {
		Serial.write(rc);

		g_rbuf[g_rcnt++] = (char_t) rc;
		if (rc == '\r') {
			Serial.write('\n');
			g_rbuf[g_rcnt++] = '\n';
			g_rbuf[g_rcnt] = '\0';
			g_line_ok = 1;
		}
	}
}
