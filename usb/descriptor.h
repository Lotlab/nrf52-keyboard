#ifndef __DESCRIPTOR_H__
#define __DESCRIPTOR_H__

/** \brief 获取USB各种描述符
 *
 * \param type1 uint8_t 描述符类型1 (hType)
 * \param type2 uint8_t 描述符类型2 (lType)
 * \param len uint8_t* 描述符长度
 * \param strPtr uint8_t** 描述符指针
 * \return bool 是否为最后一个描述符
 *
 */
uint8_t GetUsbDescriptor(uint8_t type1, uint8_t type2, uint8_t index, uint8_t *len, uint8_t **strPtr);

#endif // __DESCRIPTOR_H__
