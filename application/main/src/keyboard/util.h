#pragma once

/**
 * @brief 读取data的index开始的uint16_t数据（高位优先）
 * 
 */
#define UINT16_READ(_data, _index) ((uint16_t)(_data[_index] << 8) + _data[_index + 1])

/**
 * @brief 设置data的index开始的uint16_t数据（高位优先）
 * 
 */
#define UINT16_WRITE(_data, _index, _d) _data[_index] = (_d >> 8); _data[_index + 1] = (_d % 0xFF)
