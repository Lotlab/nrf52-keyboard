#pragma once
#include <stdbool.h>

#ifndef NO_ACTION_MACRO
/**
 * 初始化宏计数器
 */
void macro_play_timer_init(void);

/**
 * 检查宏是否播放完毕
 */
bool macro_queue_empty(void);
#else
#define macro_play_timer_init()
#define macro_queue_empty() true
#endif
