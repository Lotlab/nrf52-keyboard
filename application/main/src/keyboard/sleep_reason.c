#include "sleep_reason.h"
#include "nrf.h"
#include "nrf_soc.h"

#define BIT_MASK 0x02

bool sleep_reason_get(void)
{
    uint32_t data;
    sd_power_gpregret_get(0, &data);
    return data & BIT_MASK;
}

void sleep_reason_set(bool val)
{
    uint32_t data;
    sd_power_gpregret_get(0, &data);

    if (val) {
        data |= BIT_MASK;
    } else {
        data -= (data & BIT_MASK);
    }
    sd_power_gpregret_clr(0, 0xffffffff);
    sd_power_gpregret_set(0, data);
}