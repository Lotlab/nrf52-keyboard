#pragma once
#include "nrf_section.h"
#include "fn_define.h"

/**
 * @brief 模块的Fn处理函数
 * 
 */
typedef void (*fn_handler)(keyrecord_t* record, uint8_t id, uint8_t opt);

/**
 * @brief 定义Fn处理函数
 * 
 */
#define FN_HANDLER_DEF() NRF_SECTION_DEF(fn_handler, const fn_handler)
/**
 * @brief 定义一个Fn处理函数
 * 
 */
#define FN_HANDLER(handler)                                 \
    NRF_SECTION_ITEM_REGISTER(fn_handler, const fn_handler) \
    __pt_##handler = &handler
#define FN_HANDLER_COUNT NRF_SECTION_ITEM_COUNT(fn_handler, const fn_handler)
#define FN_HANDLER_GET(i) (NRF_SECTION_ITEM_GET(fn_handler, const fn_handler, i))
