#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*task_t)(void);

typedef struct
{
    uint16_t period;
    task_t task;
} timer_info;

/* 定义一个定时器 */
#define TIMER_DEF(callback, interval)        \
    {                                        \
        .period = interval, .task = callback \
    }

#define TIMER_STRUCT_SIZE(data) (sizeof(data) / sizeof(timer_info))

/* 定义定时器的函数 */
#define TIMER_INIT(name, timers)                                              \
    static uint16_t name##_counter[TIMER_STRUCT_SIZE(timers)];                \
    static void name##_tick()                                                 \
    {                                                                         \
        for (int i = 0; i < sizeof(name##_counter) / sizeof(uint16_t); i++)   \
            name##_counter[i]++;                                              \
    }                                                                         \
    static void name##_task_exec()                                            \
    {                                                                         \
        for (int i = 0; i < sizeof(name##_counter) / sizeof(uint16_t); i++) { \
            if (name##_counter[i] >= timers[i].period) {                      \
                name##_counter[i] = 0;                                        \
                (*(timers[i].task))();                                        \
            }                                                                 \
        }                                                                     \
    }
