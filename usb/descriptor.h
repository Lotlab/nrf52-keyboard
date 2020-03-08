#pragma once

/** \brief 获取USB各种描述符
 *
 * \param type1 uint8_t 描述符类型1 (hType)
 * \param type2 uint8_t 描述符类型2 (lType)
 * \param strPtr uint8_t** 描述符指针
 * \return uint8_t 描述符长度
 *
 */
uint8_t GetUsbDescriptor(uint8_t type1, uint8_t type2, uint8_t index, uint8_t **strPtr);
uint8_t getSerial(char* str);
