#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "ble_hids.h"
#include "ble_services.h"

void hid_service_init(ble_srv_error_handler_t err_handler);
void hid_event_handler(enum user_event evt, void* arg);
void keys_send(uint8_t report_index, uint8_t key_pattern_len, uint8_t* p_key_pattern);
bool hid_queue_empty(void);

extern uint8_t keyboard_led_val_ble;

/** Quick HID param setup macro
 * 
 * @param _name: name to setup
 * @param _len: report max length
 * @param _id: report id
 * @param _type: report type
 */
#define HID_REP_SETUP(_name, _len, _id, _type) \
    {                                          \
        _name.max_len = _len;                  \
        _name.rep_ref.report_id = _id;         \
        _name.rep_ref.report_type = _type;     \
        _name.sec.wr = SEC_CURRENT;            \
        _name.sec.rd = SEC_CURRENT;            \
    }

/** Setup Input report
 * 
 * @param _name: name to setup
 * @param _len: report max length
 * @param _id: report id
 */
#define HID_REP_IN_SETUP(_name, _len, _id)                       \
    {                                                            \
        HID_REP_SETUP(_name, _len, _id, BLE_HIDS_REP_TYPE_INPUT) \
        _name.sec.cccd_wr = SEC_CURRENT;                         \
    }

/** Setup Output report
 * 
 * @param _name: name to setup
 * @param _len: report max length
 * @param _id: report id
 */
#define HID_REP_OUT_SETUP(_name, _len, _id) HID_REP_SETUP(_name, _len, _id, BLE_HIDS_REP_TYPE_OUTPUT)

/** Setup Feature report
 * 
 * @param _name: name to setup
 * @param _len: report max length
 * @param _id: report id
 */
#define HID_REP_FEATURE_SETUP(_name, _len, _id) HID_REP_SETUP(_name, _len, _id, BLE_HIDS_REP_TYPE_FEATURE)
