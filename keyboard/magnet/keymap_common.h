#pragma once

#include <stdint.h>
#include "keymap.h"
#include "config.h"

extern const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS];
extern const action_t fn_actions[];

#define KEYMAP( \
  K00, K01, \
  K10, K11, \
  K20, K21, \
  K30, K31  \
) { \
  { K00, K01 }, \
  { K10, K11 }, \
  { K20, K21 }, \
  { K30, K31 }  \
}
