#pragma once

#include "host.h"
#include "host_driver.h"

extern host_driver_t driver;
/**
 * 待发送按键是否为空
 */
bool keys_queue_empty(void);