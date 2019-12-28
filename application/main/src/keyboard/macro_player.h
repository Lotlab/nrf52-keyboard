#pragma once
#include <stdbool.h>

#ifndef NO_ACTION_MACRO
/**
 * 播放下一个宏
 * 请每隔1ms调用一次这个
 */
void action_macro_replay(void);

/**
 * 检查宏是否播放完毕
 */
bool macro_queue_empty(void);
#else
#define action_macro_replay()
#define macro_queue_empty() true
#endif
