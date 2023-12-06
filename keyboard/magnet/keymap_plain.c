#include "keymap_common.h"
#include "keyboard_fn.h"

const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  KEYMAP(
    KC_1, KC_2,
    KC_Q, KC_W,
    KC_A, KC_S,
    KC_Z, KC_FN0
  ),
  KEYMAP(
    KC_5, KC_6,
    KC_T, KC_Y,
    KC_G, KC_H,
    KC_B, KC_TRNS
  ),
};

const action_t fn_actions[] = {
  ACTION_LAYER_MOMENTARY(1),
};
