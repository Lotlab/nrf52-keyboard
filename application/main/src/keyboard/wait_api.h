#ifndef WAIT_API_H
#define WAIT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_delay.h"
#define wait_ms(ms) nrf_delay_ms(ms)
#define wait_us(us) nrf_delay_us(us)

#ifdef __cplusplus
}
#endif

#endif
