/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include "timer.h"

#include "app_timer.h"

void timer_init()
{
    // do nothing
}

void timer_clear()
{
    // can't do anything
}

inline uint16_t timer_read()
{
    return (uint16_t)(timer_read32() % 0xFFFF);
}

inline uint32_t timer_read32()
{
    uint32_t time;
    // 24bit, 32768 tick per sec.
    // so only 19 bits are valid.

    // but, tmk does not use 32bit timer
    time = app_timer_cnt_get();
    return time / 32;
}

inline uint16_t timer_elapsed(uint16_t last)
{
    return TIMER_DIFF_16(timer_read(), last);
}

inline uint32_t timer_elapsed32(uint32_t last)
{
    uint32_t time = app_timer_cnt_get();
    uint32_t elapsed = app_timer_cnt_diff_compute(time, last * 32);
    return elapsed / 32;
}
