#pragma once
#include "config.h"
#include "nrf_section.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef ACTIONMAP_ENABLE
#define SINGLE_KEY_SIZE 2
#else
#define SINGLE_KEY_SIZE 1
#endif

#define MAX_LAYER 8 // 存储区域中最大层数
#define MAX_FN_KEYS 32 // 存储区域最大Fn数目
#define MAX_MACRO_SIZE 1024 // 宏存储区域最大容量
#define FILE_ID 0x0514

#define KEYMAP_ROW_SIZE (MATRIX_COLS * SINGLE_KEY_SIZE)
#define KEYMAP_LAYER_SIZE (MATRIX_ROWS * KEYMAP_ROW_SIZE)

/**
 * @brief 存储类型
 * 
 */
enum storage_type {
    STORAGE_KEYMAP, // Keymap
    STORAGE_FN, // Fn 区域
    STORAGE_MACRO, // 自定义宏
    STORAGE_CONFIG // 配置区域
};

void storage_read(uint8_t mask);
void storage_init(void);
bool storage_write(uint8_t mask);
void storage_delete(uint8_t mask);
uint16_t storage_read_data(enum storage_type type, uint16_t offset, uint16_t len, uint8_t* data);
uint16_t storage_write_data(enum storage_type type, uint16_t offset, uint16_t len, uint8_t* data);

// 若启用了BOOTMAGIC_ENABLE则自动启用CONFIG_STORAGE
#ifdef BOOTMAGIC_ENABLE
#define CONFIG_STORAGE
#endif

#ifdef CONFIG_STORAGE
extern uint8_t config_block[];

/**
 * @brief 配置记录区域
 * 
 */
struct config_section {
    /**
     * @brief 数据长度
     * 
     */
    uint8_t len;
    /**
     * @brief 数据指针
     * 
     */
    uint8_t* data;
};

/**
 * @brief 定义一个储存区
 * 
 */
#define CONFIG_SECTION_DEF() NRF_SECTION_DEF(config_def, struct config_section)
/**
 * @brief 定义一块数据记录区域
 * 
 */
#define CONFIG_SECTION(name, length) NRF_SECTION_ITEM_REGISTER(config_def, struct config_section) name = { .len = length };
#define CONFIG_SECTION_COUNT NRF_SECTION_ITEM_COUNT(config_def, struct config_section)
#define CONFIG_SECTION_GET(i) (NRF_SECTION_ITEM_GET(config_def, struct config_section, i))

#endif
