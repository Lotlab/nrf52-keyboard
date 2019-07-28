/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "usb_descriptor.h"
#include <stdint.h>
#include <string.h>

const uint8_t itoa[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static uint8_t __xdata descBuffer[64];

static uint8_t fillDescBuffer(char* str)
{
    uint8_t len = strlen(str);

    descBuffer[0] = len * 2 + 2;
    descBuffer[1] = 0x03;

    for (uint8_t i = 0; i < len; i++) {
        descBuffer[(i + 1) * 2] = str[i];
        descBuffer[(i + 1) * 2 + 1] = 0x00;
    }
    return len * 2 + 2;
}

static uint8_t getSerial()
{
    uint8_t i = 1;
    descBuffer[i++] = 0x03;

    for (uint16_t addr = 0x3FFC; addr <= 0x3FFF; addr++) {
        uint16_t se = (uint16_t)(*((const uint8_t __code*)(addr)));
        descBuffer[i++] = itoa[(se >> 4) % 0xF];
        descBuffer[i++] = 0x00;
        descBuffer[i++] = itoa[(se >> 0) % 0xF];
        descBuffer[i++] = 0x00;
    }

    descBuffer[0] = i;
    return i;
}

/** \brief 获取文本描述符
 *
 * \param order uint8_t 文本描述符顺序
 * \param strPtor uint8_t** 输出文本指针的指针
 * \return uint8_t 文本描述符长度
 *
 */
static uint8_t getStringDescriptor(uint8_t order, uint8_t** strPtor)
{
    uint8_t header = 0, strlen = 0;
    switch (order) {
    case 0:
        *strPtor = (uint8_t*)&LangStringDesc[0];
        strlen = LangStringDesc[0];
        break;
    case INTF_STRING_INDEX:
    case INTF_STRING_INDEX + 1:
    case INTF_STRING_INDEX + 2:
        order -= INTF_STRING_INDEX;
        do {
            header += strlen;
            if (header >= sizeof(InterfaceStringDesc)) // 超过长度就直接返回
            {
                return 0xFF;
            }
            strlen = InterfaceStringDesc[header];
        } while (order--);
        *strPtor = (uint8_t*)&InterfaceStringDesc[header];
        break;
    case 1:
        strlen = fillDescBuffer(MANUFACTURER);
        *strPtor = descBuffer;
        break;
    case 2:
        strlen = fillDescBuffer(PRODUCT);
        *strPtor = descBuffer;
        break;
    case 3:
        strlen = getSerial();
        *strPtor = descBuffer;
        break;
    default:
        strlen = 0xFF;
        break;
    }

    return strlen;
}

/** \brief 获取USB各种描述符
 *
 * \param type1 uint8_t 描述符类型1 (hType)
 * \param type2 uint8_t 描述符类型2 (lType)
 * \param strPtr uint8_t** 描述符指针
 * \return uint8_t 描述符长度
 *
 */
uint8_t GetUsbDescriptor(uint8_t type1, uint8_t type2, uint8_t index, uint8_t** strPtr)
{
    // printf_tiny("getDesc:%x, %x\n", type1, type2);
    switch (type1) {
    /**< 设备描述符 */
    case 1:
        *strPtr = (uint8_t*)&DeviceDescriptor[0];
        return sizeof(DeviceDescriptor);
    /**< 配置描述符 */
    case 2:
        *strPtr = (uint8_t*)&ConfigDescriptor[0];
        return sizeof(ConfigDescriptor);
    /**< 字符串描述符 */
    case 3:
        return getStringDescriptor(type2, strPtr);
    /**< HID 报告描述符 */
    case 0x22:
        if (index < sizeof(ReportDescriptor)) {
            *strPtr = ReportDescriptor[index].pointer;
            return ReportDescriptor[index].length;
        } else {
            // 接口超出上限
            return 0xff;
        }

    default:
        return 0xff; //不支持的命令或者出错
    }
}
