#pragma once
#include <stdint.h>
#include "main.h"

#include <stdlib.h>
#include "nrf_section.h"

void execute_event(void);

/**
 * @brief UserEventHandler 是自定义的事件处理函数
 * 
 * @param uint8_t code: 事件代码，见events.h内的 enum user_event 定义
 * @param void* arg: 事件参数的指针，详见文档。
 */
typedef void (*UserEventHandler)(uint8_t, void*);

/**
 * @brief 将一个函数注册为事件处理函数
 */
#define EVENT_HANDLER(_func) NRF_SECTION_ITEM_REGISTER(modules_init, const UserEventHandler _pt_##_func) = &_func
