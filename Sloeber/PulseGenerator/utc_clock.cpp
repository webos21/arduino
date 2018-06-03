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

#include "utc_clock.h"

#include "aproject.h"
#include "serial_adapter.h"

#include "Arduino.h"
#include "MsTimer2.h"

/* ----------------------------------------------
 * Internal Structures
 * ---------------------------------------------- */

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

#define DATE_MAX        2147483647L
#define TIME_MAX        0xffffffffUL
#define INITIAL_YR      1970
#define INITIAL_WDAY    4
#define SECS_DAY        86400
#define YEAR0           1900
#define LEAPYEAR(year)  ((((year) % 4) == 0) && ((((year) % 100) != 0) || (((year) % 400) == 0)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)

typedef struct s_tspec_t {
	NOPT int32_t sec;
	NOPT int16_t milli;
} tspec_t;

static const uint8_t g_ytab[2][12] = { { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
		30, 31 }, { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } };

static NOPT int32_t g_diff_sec = 0L;
static NOPT int32_t g_tv_sec = 0L;
static NOPT int32_t g_tv_milli = 0L;

static const char_t TZ_KST[] = "KST";

#ifndef NULL
#define NULL ((void*)0)
#endif

/* ----------------------------------------------
 * Internal Functions
 * ---------------------------------------------- */

/* Forward Declaration : Begin */
static int32_t utc_gmtime(register int32_t ntt, utc_ts_t *ts);
static int32_t utc_mktime(register utc_ts_t *ts);
static int32_t /*@alt void@*/stx_gettime(int32_t clockId, tspec_t *ts);
static int32_t utc_settime(int32_t clockId, const tspec_t *ts);
static void HAL_SYSTICK_Callback(void);
/* Forward Declaration : End */

static int32_t utc_gmtime(register int32_t ntt, utc_ts_t *ts) {
	register int32_t dayclock, dayno;
	int16_t year = INITIAL_YR;

	int32_t rc = -1L;

	if (ts != NULL) {
		dayclock = ntt % SECS_DAY;
		dayno = ntt / SECS_DAY;

		ts->sec = (int8_t) (dayclock % 60);
		ts->min = (int8_t) ((dayclock % 3600) / 60);
		ts->hour = (int8_t) (dayclock / 3600);
		while (dayno >= YEARSIZE(year)) {
			dayno -= YEARSIZE(year);
			year++;
		}
		ts->year = year;
		ts->mon = 0;
		while (dayno >= g_ytab[LEAPYEAR(year)][ts->mon]) {
			dayno -= g_ytab[LEAPYEAR(year)][ts->mon];
			ts->mon++;
		}
		ts->mon++;
		ts->day = (int8_t) (dayno + 1);
		ts->wday = (dayno + INITIAL_WDAY) % 7;
		ts->msec = 0;

		rc = 0L;
	}

	return rc;
}

static int32_t utc_mktime(register utc_ts_t *ts) {
	register int32_t day, year;
	register int32_t ts_year;
	int32_t yday, month;
	register int32_t seconds;
	int32_t overflow;

	int32_t rc = -1L;

	ts->mon -= 1;
	ts->year -= YEAR0;

	ts->min += ts->sec / 60;
	ts->sec %= 60;
	if (ts->sec < 0) {
		ts->sec += 60;
		ts->min--;
	}
	ts->hour += ts->min / 60;
	ts->min = ts->min % 60;
	if (ts->min < 0) {
		ts->min += 60;
		ts->hour--;
	}
	day = ts->hour / 24;
	ts->hour = ts->hour % 24;
	if (ts->hour < 0) {
		ts->hour += 24;
		day--;
	}
	ts->year += ts->mon / 12;
	ts->mon %= 12;
	if (ts->mon < 0) {
		ts->mon += 12;
		ts->year--;
	}
	day += (ts->day - 1);
	while (day < 0) {
		if (--ts->mon < 0) {
			ts->year--;
			ts->mon = 11;
		}
		day += g_ytab[LEAPYEAR(YEAR0 + ts->year)][ts->mon];
	}
	while (day >= g_ytab[LEAPYEAR(YEAR0 + ts->year)][ts->mon]) {
		day -= g_ytab[LEAPYEAR(YEAR0 + ts->year)][ts->mon];
		if (++(ts->mon) == 12) {
			ts->mon = 0;
			ts->year++;
		}
	}
	ts->day = day + 1;

	year = INITIAL_YR;
	if (ts->year > (year - YEAR0)) {
		seconds = 0;
		day = 0; /* means days since day 0 now */
		overflow = 0;

		ts_year = ts->year + YEAR0;

		if (DATE_MAX / 365 < ts_year - year) {
			overflow++;
		}
		day = (ts_year - year) * 365;
		if (DATE_MAX - day < (ts_year - year) / 4 + 1) {
			overflow++;
		}
		day += ((ts_year - year) / 4)
				+ (((ts_year % 4) != 0) && ((ts_year % 4) < (year % 4)));
		day -= ((ts_year - year) / 100)
				+ (((ts_year % 100) != 0) && ((ts_year % 100) < (year % 100)));
		day += ((ts_year - year) / 400)
				+ (((ts_year % 400) != 0) && ((ts_year % 400) < (year % 400)));

		yday = month = 0;
		while (month < ts->mon) {
			yday += g_ytab[LEAPYEAR(ts_year)][month];
			month++;
		}
		yday += (ts->day - 1);
		if (day + yday < 0) {
			overflow++;
		}
		day += yday;

		seconds = (long) (((ts->hour * 60L) + ts->min) * 60L + ts->sec);

		if ((int32_t) ((TIME_MAX - (uint32_t) seconds) / SECS_DAY) < day) {
			overflow++;
		}
		seconds += day * SECS_DAY;

		/* Restore Value */
		ts->mon += 1;
		ts->year += YEAR0;

		if ((overflow == 0L) && ((long) seconds == seconds)) {
			rc = seconds;
		}
	}

	return rc;
}

static int32_t stx_gettime(int32_t clockId, tspec_t *ts) {
	int32_t rc = -1L;

	if (ts != NULL) {
		if (CLOCK_REALTIME == clockId) {
			rc = g_tv_sec;
			rc += g_diff_sec;
			ts->sec = rc;
			ts->milli = g_tv_milli;
		} else { /* if (clockId == CLOCK_MONOTONIC)  */
			ts->sec = g_tv_sec;
			ts->milli = g_tv_milli;
		}
		rc = 0L;
	}

	return rc;
}

static int32_t utc_settime(int32_t clockId, const tspec_t *ts) {
	int32_t rc = -1L;

	if (ts != NULL) {
		if (clockId == CLOCK_REALTIME) {
			rc = ts->sec;
			rc -= g_tv_sec;
			g_diff_sec = rc;
			g_tv_milli = ts->milli;
		} else { /* if (clockId == CLOCK_MONOTONIC)  */
			g_tv_sec = ts->sec;
			g_tv_milli = ts->milli;
		}
		rc = 0L;
	}

	return rc;
}

static void HAL_SYSTICK_Callback(void) {
	g_tv_milli++;
	if (g_tv_milli > 999L) {
		g_tv_milli = 0L;
		g_tv_sec++;
	}
}

/* ----------------------------------------------
 * Interface Implementations
 * ---------------------------------------------- */

void utc_clock_init(void) {
	MsTimer2::set(1, &HAL_SYSTICK_Callback);
	MsTimer2::start();
}

int32_t utc_clock_sec(void) {
	int32_t rv = g_tv_sec;
	rv += g_diff_sec;
	return rv;
}

int64_t utc_clock_msec(void) {
	int64_t rv = g_tv_sec;
	rv += g_diff_sec;
	rv *= 1000L;
	rv += g_tv_milli;
	return rv;
}

int64_t utc_clock_tick(void) {
	int64_t rv = g_tv_sec;
	rv *= 1000L;
	rv += g_tv_milli;
	return rv;
}

int32_t utc_clock_tick_sec(void) {
	return g_tv_sec;
}

int64_t utc_clock_ts2msec(utc_ts_t *ts) {
	int64_t pt;
	int32_t tt;

	tt = utc_mktime(ts);
	pt = tt;
	pt *= 1000L;
	pt += ts->msec;

	return pt;
}

int32_t utc_clock_ts2sec(utc_ts_t *ts) {
	return utc_mktime(ts);
}

int32_t utc_clock_msec2ts(int64_t msec, utc_ts_t *ts) {
	int32_t rc;
	int32_t tt = (msec / 1000L);

	rc = utc_gmtime(tt, ts);
	if (rc == 0L) {
		ts->msec = (msec % 1000L);
		ts->tz = TZ_KST;
	}

	return rc;
}

int32_t utc_clock_sec2ts(int32_t sec, utc_ts_t *ts) {
	int32_t rc;

	rc = utc_gmtime(sec, ts);
	if (rc == 0L) {
		ts->msec = 0;
		ts->tz = TZ_KST;
	}

	return rc;
}

int32_t utc_clock_ts(utc_ts_t *ts) {
	tspec_t now;
	int32_t rc;

	stx_gettime(CLOCK_REALTIME, &now);

	rc = utc_gmtime(now.sec, ts);
	if (rc == 0L) {
		ts->msec = now.milli;
		ts->tz = TZ_KST;
	}

	return rc;
}

int32_t utc_clock_set(utc_ts_t *ts) {
	tspec_t now;

	now.sec = utc_clock_ts2sec(ts);
	now.milli = ts->msec;

	return utc_settime(CLOCK_REALTIME, &now);
}

void utc_clock_ts_print(const utc_ts_t *ts) {
	serial_print(FC("[TimeStamp] %04d-%02d-%02d %02d:%02d:%02d.%03d\r\n"), ts->year,
			ts->mon, ts->day, ts->hour, ts->min, ts->sec, ts->msec);
}
