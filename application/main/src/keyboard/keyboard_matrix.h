#pragma once
#include <stdint.h>

void matrix_deinit(void);
void matrix_wakeup_prepare(void);

#ifdef MATRIX_FORIGN_KEY
void matrix_forign_add_oneshot(uint8_t row, uint8_t col);
void matrix_forign_set(uint8_t row, uint8_t col, bool press);
#endif
