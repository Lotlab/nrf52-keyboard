#if !defined(__KEYMAP_STORAGE_H__)
#define __KEYMAP_STORAGE_H__

#include <stdint.h>

void keymap_init(void);
void keymap_set(uint8_t block, uint8_t size, uint8_t* data);
void keymap_write(void);

#endif // __KEYMAP_STORAGE_H__
