#include "app_timer.h"

#define MAX_TIMER_SIZE 3
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

void timer_create(void *task, bool repeat, uint16_t period)
{
    timer_info *timer = &timers[pos++];
    timer->exec_flag = false;
    timer->is_start = true;
    timer->period = period;
    timer->repeat = repeat;
    timer->task = task;
    timer->time_count = 0;
}
