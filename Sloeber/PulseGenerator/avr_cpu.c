/*
 * Copyright 2016 Cheolmin Jo (webos21@gmail.com)
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

#include "avr_cpu.h"

#include "serial_adapter.h"

#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* declare reset function @ address 0 */
void (*avr_cpu_reset_fn)(void) = 0;

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void avr_cpu_id(uint8_t cpuId[/*3*/]) {
	cpuId[0] = boot_signature_byte_get(0);
	cpuId[1] = boot_signature_byte_get(2);
	cpuId[2] = boot_signature_byte_get(4);
}

uint16_t avr_cpu_flash_size(void) {
	uint16_t v = 0;
	uint8_t x = boot_signature_byte_get(2);
	switch (x) {
	case 0x94:
		v = 16;
		break;
	case 0x95:
		v = 32;
		break;
	case 0x97:
		v = 128;
		break;
	case 0x98:
		v = 256;
		break;
	default:
		v = 0;
		break;
	}
	return v;
}

void avr_cpu_enable_interrupt(void) {
	sei();
}

void avr_cpu_disable_interrupt(void) {
	cli();
}

void avr_cpu_reboot(void) {
	/* WARN : Arduino NANO's old bootloader is not working!! */
	/* WARN : upgrade the bootloader of Arduino NANO */

#if 1 /* Reboot by WatchDog */
	/* irq's off */
	cli();

	/* WatchDog ON, 15 msec */
	wdt_reset();
	wdt_enable(WDTO_15MS);

	/* Reset Loop */
	while (1) {
		/* Nothing to do */
	}
#endif /* Reboot by WatchDog */

#if 0 /* Reboot by JUMP to 0 */
	avr_cpu_reset_fn();
#endif /* Reboot by JUMP to 0 */

#if 1 /* If above action is all filed : Not supported */
	serial_print("avr_cpu_reboot() is not supported!!!\r\n");
#endif
}
