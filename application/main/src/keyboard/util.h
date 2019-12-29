#pragma once

/**
 * @brief 读取data的index开始的uint16_t数据（小端序）
 * 
 */
#define UINT16_READ(_data, _index) (_data[_index] + (uint16_t)(_data[_index + 1] << 8))

/**
 * @brief 设置data的index开始的uint16_t数据（小端序）
 * 
 */
#define UINT16_WRITE(_data, _index, _d) _data[_index] = (_d & 0xFF); _data[_index + 1] = (_d >> 8)

/**
 * @brief uint16_t转换为uint8_t序列（小端序）
 * 
 */
#define UINT16_SEQ(_d) (_d & 0xFF), ((_d >> 8) & 0xFF)

/**
 * @brief uint32_t转换为uint8_t序列（小端序）
 * 
 */
#define UINT32_SEQ(_d) (_d & 0xFF), ((_d >> 8) & 0xFF), ((_d >> 16) & 0xFF), ((_d >> 24) & 0xFF)