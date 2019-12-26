#pragma once

#include "ble.h"
#include "../main.h"
#include <stdbool.h>
#include <stdint.h>

extern uint16_t m_conn_handle;

/**@brief Init basic ble services
 */
void ble_services_init();

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

/**@brief Send passkey.
 */
void ble_passkey_send(uint8_t const * p_key);

void advertising_slow(void);
