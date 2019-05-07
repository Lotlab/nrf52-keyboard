#include "eeconfig.h"

void eeconfig_init(void)
{
#ifdef BACKLIGHT_ENABLE
    // todo
#endif
}

void eeconfig_enable(void)
{
    // todo
}

void eeconfig_disable(void)
{
    // todo
}

bool eeconfig_is_enabled(void)
{
    // todo
    return false;
}

uint8_t eeconfig_read_debug(void)
{
    // todo
    return 0;
}
void eeconfig_write_debug(uint8_t val)
{
    // todo
}

uint8_t eeconfig_read_default_layer(void)
{
    // todo
    return 0;
}
void eeconfig_write_default_layer(uint8_t val)
{
    // todo
}

uint8_t eeconfig_read_keymap(void)
{
    // todo
    return 0;
}
void eeconfig_write_keymap(uint8_t val)
{
    // todo
}

#ifdef BACKLIGHT_ENABLE
uint8_t eeconfig_read_backlight(void)
{
    // todo
    return 0;
}
void eeconfig_write_backlight(uint8_t val)
{
    // todo
}
#endif