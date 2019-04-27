#ifndef __BLE_SERVICES__
#define __BLE_SERVICES__

#include "ble.h"
#include <stdbool.h>
#include <stdint.h>

extern uint16_t m_conn_handle;

enum user_ble_event {
    USER_BLE_IDLE,
    USER_BLE_DISCONNECT,
    USER_BLE_GATTS_TX_COMPLETE
};

typedef void (*evt_handler) (enum user_ble_event arg);

/**@brief Init basic ble services
 */
void ble_services_init(evt_handler handler);

// void auth_key_reply(uint8_t* passkey);
// bool auth_key_reqired(void);

/**@brief Clear bond information from persistent storage.
 */
void delete_bonds(void);

/**@brief Init Bluetooth stack
 */
void ble_stack_init(void);

/**@brief Start Adv.
 */
void advertising_start(bool erase_bonds);

#endif
