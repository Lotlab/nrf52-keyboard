#pragma once

#include <stdint.h>
#include "ble_services.h"
#include "ble_hids.h"

void hid_service_init(ble_srv_error_handler_t err_handler);
void hid_event_handler(enum user_ble_event arg);
void keys_send(uint8_t report_index, uint8_t key_pattern_len, uint8_t* p_key_pattern);

extern uint8_t keyboard_led_val_ble;
