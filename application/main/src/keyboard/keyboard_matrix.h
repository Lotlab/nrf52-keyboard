#pragma once
#include <stdint.h>
#include "matrix.h"

void matrix_init(void);
matrix_row_t read_cols(void);
void select_row(uint8_t row);
void unselect_rows(uint8_t row);
uint8_t matrix_scan(void);
bool matrix_is_modified(void);
void matrix_deinit(void);
void matrix_wakeup_prepare(void);

#ifdef MATRIX_FORIGN_KEY
void matrix_forign_add_oneshot(uint8_t row, uint8_t col);
void matrix_forign_set(uint8_t row, uint8_t col, bool press);
#endif
