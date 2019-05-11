#ifndef __BLE_SERVICES__
#define __BLE_SERVICES__

#include "ble.h"
#include <stdbool.h>
#include <stdint.h>

extern uint16_t m_conn_handle;

/**
 * @brief 蓝牙用户事件
 * 
 */
enum user_ble_event {
    USER_BLE_IDLE, // 蓝牙进入空闲状态
    USER_BLE_DISCONNECT, // 蓝牙被断开
    USER_BLE_GATTS_TX_COMPLETE, // GATTs发送完毕
    USER_BLE_CONNECTED, // 蓝牙已连接
    USER_BLE_PASSKEY, // 需要Passkey
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

/**@brief Send passkey.
 */
void ble_passkey_send(uint8_t const * p_key);

#endif
