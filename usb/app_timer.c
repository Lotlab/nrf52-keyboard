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

#include "app_timer.h"

#define MAX_TIMER_SIZE 2
volatile __idata timer_info timers[MAX_TIMER_SIZE];
uint8_t pos = 0;

void timer_tick()
{
    for (int i = 0; i < pos; i++)
    {
        timer_info *timer = &timers[i];
        if (timer->is_start)
        {
            timer->time_count++;
            if (timer->time_count >= timer->period)
            {
                timer->time_count = 0;
                timer->exec_flag = true;
                if (!timer->repeat)
                {
                    timer->is_start = false;
                }
            }
        }
    }
}

void timer_task_exec()
{
    for (int i = 0; i < pos; i++)
    {
        timer_info *timer = &timers[i];
        if (timer->exec_flag)
        {
            (*(timer->task))();
            timer->exec_flag = false;
        }
    }
}

void timer_create(task_t task, bool repeat, uint16_t period)
{
    timer_info *timer = &timers[pos++];
    timer->exec_flag = false;
    timer->is_start = true;
    timer->period = period;
    timer->repeat = repeat;
    timer->task = task;
    timer->time_count = 0;
}
