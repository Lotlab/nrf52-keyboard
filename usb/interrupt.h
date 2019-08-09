#pragma once

#include <stdbool.h>

/** \brief USB 中断服务函数
 *
 * \return void
 *
 */
void UsbIsr();
void UsbSuspendEvt(bool sleep);
