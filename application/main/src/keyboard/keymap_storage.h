#if !defined(__KEYMAP_STORAGE_H__)
#define __KEYMAP_STORAGE_H__

#include <stdint.h>
#include <stdbool.h>

void keymap_init(void);
bool keymap_set(uint8_t block, uint8_t size, uint8_t* data);

#endif // __KEYMAP_STORAGE_H__
