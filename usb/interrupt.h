#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdbool.h>

/** \brief USB 中断服务函数
 *
 * \return void
 *
 */
void UsbIsr();
void UsbSuspendEvt(bool sleep);

#endif // __INTERRUPT_H__
