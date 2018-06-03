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

#ifndef ATMEGA328_TIMER0_H
#define ATMEGA328_TIMER0_H

/* -----------------------------------------
 * The [Timer0] of ATmega328P (Arduino UNO / NANO)
 * ----------------------------------------- */

/*
 * The ATmega328P has 3 Timers and 6 PWM output pins 3, 5, 6, 9, 10, and 11
 *
 * - [timer 0] : Pins 5, 6  / time functions like millis(), and delay()
 * - [timer 1] : Pins 9, 10 / servo library
 * - [timer 2] : Pins 11, 3
 */

/*
 * So, WE DO NOT USE [timer 0]
 */

#endif /* ATMEGA328_TIMER0_H */
