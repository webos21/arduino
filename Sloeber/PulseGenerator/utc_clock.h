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

#ifndef UTC_CLOCK_H
#define UTC_CLOCK_H

/* -----------------------------------------
 * The Clock functions for STM32
 * ----------------------------------------- */

#include "aproject.h"

/*
 * Start C++ Declaration
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_utc_ts_t {
	int16_t year;
	int8_t mon;
	int8_t day;
	int8_t hour;
	int8_t min;
	int8_t sec;
	int16_t msec;
	int8_t wday;
	/*@observer@*/
	const char_t * tz;
} utc_ts_t;

void      utc_clock_init(void);

int32_t   utc_clock_sec(void);
int64_t   utc_clock_msec(void);
int64_t   utc_clock_tick(void);
int32_t   utc_clock_tick_sec(void);
int64_t   utc_clock_ts2msec(utc_ts_t *ts);
int32_t   utc_clock_ts2sec(utc_ts_t *ts);
int32_t   utc_clock_msec2ts(int64_t msec, utc_ts_t *ts);
int32_t   /*@alt void@*/ utc_clock_sec2ts(int32_t sec, utc_ts_t *ts);
int32_t   /*@alt void@*/ utc_clock_ts(utc_ts_t *ts);
int32_t   /*@alt void@*/ utc_clock_set(utc_ts_t *ts);
void      utc_clock_ts_print(const utc_ts_t *ts);

/*
 * End C++ Declaration
 */
#ifdef __cplusplus
}
#endif

#endif /* UTC_CLOCK_H */
