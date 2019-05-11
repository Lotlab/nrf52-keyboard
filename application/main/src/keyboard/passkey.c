#include "passkey.h"
#include "custom_hook.h"
#include "../ble/ble_services.h"

static uint8_t inputed_len = 0xFF;
static uint8_t passkey[6];

void hook_send_keyboard(report_keyboard_t* report)
{
    if (inputed_len < 6) {
        for (uint8_t i = 0; i < sizeof(report->keys); i++) {
            uint8_t keycode = report->keys[i];
            if (keycode >= KC_1 && keycode <= KC_0) {
                passkey[inputed_len++] = (keycode + 1 - KC_1) % 10 + '0';
                break;
            } else if (keycode >= KC_KP_1 && keycode <= KC_KP_0) {
                passkey[inputed_len++] = (keycode + 1 - KC_KP_1) % 10 + '0';
                break;
            } else if (keycode == KC_BSPACE && inputed_len > 0) {
                inputed_len--;
                break;
            }
        }
        if (inputed_len == 6) {
            ble_passkey_send(passkey);
            inputed_len = 0xFF;
        }
    }
}

void passkey_req_handler() {
    inputed_len = 0;
}