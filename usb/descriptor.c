#include <stdint.h>
#include "usb_descriptor.h"
// #include <stdio.h>

/** \brief 获取文本描述符
 *
 * \param order uint8_t 文本描述符顺序
 * \param strPtor uint8_t** 输出文本指针的指针
 * \return uint8_t 文本描述符长度
 *
 */
static uint8_t getStringDescriptor(uint8_t order, uint8_t **strPtor)
{
    uint8_t header = 0, strlen = 0;
    do
    {
        header += strlen;
        if (header >= sizeof(StringDescriptor)) // 超过长度就直接返回
        {
            return 0xFF;
        }
        strlen = StringDescriptor[header];
    } while (order--);

    *strPtor = (uint8_t *)&StringDescriptor[header];
    return strlen;
}

/** \brief 获取USB各种描述符
 *
 * \param type1 uint8_t 描述符类型1 (hType)
 * \param type2 uint8_t 描述符类型2 (lType)
 * \param len uint8_t* 描述符长度
 * \param strPtr uint8_t** 描述符指针
 * \return uint8_t 是否为最后一个描述符
 *
 */
uint8_t GetUsbDescriptor(uint8_t type1, uint8_t type2, uint8_t index, uint8_t *len, uint8_t **strPtr)
{
    // printf_tiny("getDesc:%x, %x\n", type1, type2);
    switch (type1)
    {
    /**< 设备描述符 */
    case 1:
        *strPtr = (uint8_t *)&DeviceDescriptor[0];
        *len = sizeof(DeviceDescriptor);
        break;
    /**< 配置描述符 */
    case 2:
        *strPtr = (uint8_t *)&ConfigDescriptor[0];
        *len = sizeof(ConfigDescriptor);
        break;
    /**< 字符串描述符 */
    case 3:
        *len = getStringDescriptor(type2, strPtr);
        break;
    /**< HID 报告描述符 */
    case 0x22:
        if (index < sizeof(ReportDescriptor))
        {
            *strPtr = ReportDescriptor[index].pointer;
            *len = ReportDescriptor[index].length;

            if (index == sizeof(ReportDescriptor) - 1)
            {
                return 1; // 设备准备就绪
            }
        }
        else // 接口超出上限
        {
            *len = 0xff;
        }
        break;

    default:
        *len = 0xff; //不支持的命令或者出错
        break;
    }
    return 0;
}
