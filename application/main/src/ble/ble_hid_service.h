#ifndef __BLE_HID_SERVICE_H__
#define __BLE_HID_SERVICE_H__

#include <stdint.h>
#include "ble_services.h"
#include "ble_hids.h"

void hid_service_init(ble_srv_error_handler_t err_handler);
void hid_event_handler(enum user_ble_event arg);
void keys_send(uint8_t key_pattern_len, uint8_t * p_key_pattern);

#endif //__BLE_HID_SERVICE_H__
