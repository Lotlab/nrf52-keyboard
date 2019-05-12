#ifndef __APP_TIMER_H_
#define __APP_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint16_t period;
    uint16_t time_count;
    bool repeat;
    bool is_start;
    bool exec_flag;
    void (*task)(void);
} timer_info;

void timer_tick(void);
void timer_task_exec(void);
void timer_create(void *task, bool repeat, uint16_t period);

#endif // __APP_TIMER_H_
