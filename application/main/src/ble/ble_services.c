/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ble_services.h"

#include "app_error.h"
#include "app_timer.h"
#include "ble.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "ble_dis.h"
#include "ble_srv_common.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_bootloader_info.h"
#include "nrf_power.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "peer_manager_handler.h"

#include "ble_config.h"
#include "data_storage.h"
#include "util.h"
#include <string.h>

#define PNP_ID_VENDOR_ID_SOURCE 0x02 /**< Vendor ID Source. */

#define APP_BLE_OBSERVER_PRIO 3 /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1 /**< A tag identifying the SoftDevice BLE configuration. */

#define FIRST_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(5000) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3 /**< Number of attempts before giving up the connection parameter negotiation. */

#define APP_ADV_FAST_INTERVAL 0x0028 /**< Fast advertising interval (in units of 0.625 ms. This value corresponds to 25 ms.). */
#define APP_ADV_SLOW_INTERVAL 0x0C80 /**< Slow advertising interval (in units of 0.625 ms. This value corrsponds to 2 seconds). */

#define APP_ADV_FAST_DURATION 3000 /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_DURATION 18000 /**< The advertising duration of slow advertising in units of 10 milliseconds. */

/*lint -emacro(524, MIN_CONN_INTERVAL) // Loss of precision */
#define MIN_CONN_INTERVAL MSEC_TO_UNITS(7.5, UNIT_1_25_MS) /**< Minimum connection interval (7.5 ms) */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(30, UNIT_1_25_MS) /**< Maximum connection interval (30 ms). */
#define SLAVE_LATENCY 6 /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(430, UNIT_10_MS) /**< Connection supervisory timeout (430 ms). */

uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */
static pm_peer_id_t m_peer_id; /**< Device reference handle to the current bonded central. */
#ifdef MULTI_DEVICE_SWITCH
uint8_t switch_id = 0; /** 当前设备ID Device ID of currently in the eeconfig   */
#endif

NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr); /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */

static ble_uuid_t m_adv_uuids[] = { { BLE_UUID_HUMAN_INTERFACE_DEVICE_SERVICE, BLE_UUID_TYPE_BLE } };


/**@brief Function for setting filtered whitelist.
 *
 * @param[in] skip  Filter passed to @ref pm_peer_id_list.
 */
static void whitelist_set(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
    uint32_t peer_id_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
    APP_ERROR_CHECK(err_code);

    err_code = pm_whitelist_set(peer_ids, peer_id_count);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for setting filtered device identities.
 *
 * @param[in] skip  Filter passed to @ref pm_peer_id_list.
 */
static void identities_set(pm_peer_id_list_skip_t skip)
{
    pm_peer_id_t peer_ids[BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT];
    uint32_t peer_id_count = BLE_GAP_DEVICE_IDENTITIES_MAX_COUNT;

    ret_code_t err_code = pm_peer_id_list(peer_ids, &peer_id_count, PM_PEER_ID_INVALID, skip);
    APP_ERROR_CHECK(err_code);

    err_code = pm_device_identities_list_set(peer_ids, peer_id_count);
    APP_ERROR_CHECK(err_code);
}

static void ble_disconnect()
{
    sd_ble_gap_adv_stop(m_advertising.adv_handle);
    if (m_conn_handle != BLE_CONN_HANDLE_INVALID) {
        ret_code_t err_code = sd_ble_gap_disconnect(m_conn_handle,
            BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        if (err_code != NRF_ERROR_INVALID_STATE) {
            APP_ERROR_CHECK(err_code);
        }
    }
}

#ifndef MULTI_DEVICE_SWITCH
static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void* p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED) {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) = {
    .handler = buttonless_dfu_sdh_state_observer,
};
#endif

#ifdef MACADDR_SEPRATOR
/**@brief MAC地址转换为设备名后缀.
 *
 * 读取MAC地址，并将其转换后放置到设备名最后
 */
static void get_device_name(char* device_name, int offset)
{
    const char lookup_table[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    strcpy(device_name, DEVICE_NAME);
    device_name[offset++] = MACADDR_SEPRATOR;

    ble_gap_addr_t ble_addr;
    sd_ble_gap_addr_get(&ble_addr);

    for (uint8_t i = 0; i < 3; i++) {
        uint8_t addr = ble_addr.addr[3 + i];
        device_name[offset++] = lookup_table[addr / 16];
        device_name[offset++] = lookup_table[addr % 16];
    }
    device_name[offset] = 0x00;
}
#endif

static void set_device_name(void)
{
    ret_code_t err_code;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

#ifdef MACADDR_SEPRATOR
    int orig_len = strlen(DEVICE_NAME);
    int name_len = orig_len + 8;
    char device_name[name_len];
    get_device_name(device_name, orig_len);

    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t*)device_name, strlen(device_name));
#else
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t*)DEVICE_NAME, strlen(DEVICE_NAME));
#endif
    APP_ERROR_CHECK(err_code);
}
/**@brief 清空所有绑定数据.
 */
void delete_bonds(void)
{
    ret_code_t err_code;

    ble_disconnect();

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
#ifdef MULTI_DEVICE_SWITCH
    //清空所有绑定后，自动回到首个设备
    switch_id = 0;
    switch_device_id_write(switch_id);
    switch_device_select(switch_id);
#endif
}

#ifdef MULTI_DEVICE_SWITCH
//注册switch需要的存储区
CONFIG_SECTION(switch_device_id, 1);
/**@brief 读取switch id.
 *
 */
uint8_t switch_device_id_read(void)
{
    return switch_device_id.data[0];
}
/**@brief 写入switch id.
 *
 */
void switch_device_id_write(uint8_t val)
{
    if (switch_device_id.data[0] != val) {
        switch_device_id.data[0] = val;
        storage_write((1 << STORAGE_CONFIG));
    }
}

/**@brief 删除当前设备绑定数据.
 *
 * 查找并删除与当前gap addr绑定的绑定数据
 */
static void peer_list_find_and_delete_bond(void)
{
    pm_peer_id_t peer_id;

    peer_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);

    while (peer_id != PM_PEER_ID_INVALID) {
        uint16_t length = 8;
        uint8_t addr[8];

        if (pm_peer_data_app_data_load(peer_id, addr, &length) == NRF_SUCCESS) {
            if (addr[3] == switch_id) {
                pm_peer_delete(peer_id);
            }
        }

        peer_id = pm_next_peer_id_get(peer_id);
    }
}

/**@brief 切换连接设备.
 *
 * @param[in] id  要切换的设备的ID号
 */
void switch_device_select(uint8_t id)
{
    ret_code_t ret;
    ble_gap_addr_t gap_addr;
    if (id == switch_id) {
        return; //如果重复切换，则直接退出，不做任何操作
    } else {
        ble_disconnect();
    }
    switch_id = id;

    switch_device_id_write(id);

    ret = sd_ble_gap_addr_get(&gap_addr);
    APP_ERROR_CHECK(ret);

    gap_addr.addr[3] = id;

    ret = sd_ble_gap_addr_set(&gap_addr);
    APP_ERROR_CHECK(ret);
}
/**@brief 重新绑定当前设备.
 *
 */
void switch_device_rebond()
{
    peer_list_find_and_delete_bond();
    uint8_t rebond_id = switch_id;
    switch_id = 10; //将switch_id设置为无效ID
    switch_device_select(rebond_id);
}
/**@brief 切换连接设备初始化.
 *
 * 读取存储的当前连接设备ID，并激活为当前连接设备
 */
static void switch_device_init()
{

    ret_code_t ret;
    ble_gap_addr_t gap_addr;
    switch_id = switch_device_id_read();

    ret = sd_ble_gap_addr_get(&gap_addr);
    APP_ERROR_CHECK(ret);

    gap_addr.addr[3] = switch_id;

    ret = sd_ble_gap_addr_set(&gap_addr);
    APP_ERROR_CHECK(ret);
}
/**@brief 更新切换设备数据.
 *
 * 获取当前gap addr并更新PM数据
 */
static void switch_device_update(pm_peer_id_t peer_id)
{
    uint32_t err_code;
    ble_gap_addr_t gap_addr;
    uint16_t length = 8;
    uint8_t addr[8];

    err_code = pm_id_addr_get(&gap_addr);
    APP_ERROR_CHECK(err_code);

    for (uint8_t i = 0; i < BLE_GAP_ADDR_LEN; i++)
        addr[i] = gap_addr.addr[i];

    addr[3] = switch_id;

    err_code = pm_peer_data_app_data_store(m_peer_id, addr, length, NULL);
    APP_ERROR_CHECK(err_code);
}
#endif

/**@brief 开启蓝牙广播.
 * 
 * @param[in] erase_bonds  是否清空所有绑定数据
 */
void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true) {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    } else {
        whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);
#ifdef MULTI_DEVICE_SWITCH
        switch_device_init();
#endif
        ret_code_t ret = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(ret);
    }
}
/**@brief 重新开启蓝牙广播.
 * 
 * @param[in] mode  广播模式
 * @param[in] reset  是否重新绑定
 */
void advertising_restart(ble_adv_mode_t mode, bool reset)
{
    if (m_conn_handle == BLE_CONN_HANDLE_INVALID) {
        sd_ble_gap_adv_stop(m_advertising.adv_handle);
        ble_advertising_start(&m_advertising, mode);
    } else {
        sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    }

    if (reset) {
        ble_advertising_restart_without_whitelist(&m_advertising);
    }
}

void advertising_slow()
{
    ret_code_t ret = ble_advertising_start(&m_advertising, BLE_ADV_MODE_SLOW);
    APP_ERROR_CHECK(ret);
}

#ifndef MULTI_DEVICE_SWITCH
static void disconnect(uint16_t conn_handle, void* p_context)
{
    UNUSED_PARAMETER(p_context);

    sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
}
#endif

static void advertising_config_get(ble_adv_modes_config_t* p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));
#ifdef MULTI_DEVICE_SWITCH
    p_config->ble_adv_whitelist_enabled = true;
#else
    p_config->ble_adv_whitelist_enabled = false;
#endif
    p_config->ble_adv_directed_high_duty_enabled = true;
    p_config->ble_adv_directed_enabled = false;
    p_config->ble_adv_directed_interval = 0;
    p_config->ble_adv_directed_timeout = 0;
    p_config->ble_adv_fast_enabled = true;
    p_config->ble_adv_fast_interval = APP_ADV_FAST_INTERVAL;
    p_config->ble_adv_fast_timeout = APP_ADV_FAST_DURATION;
    p_config->ble_adv_slow_enabled = true;
    p_config->ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
    p_config->ble_adv_slow_timeout = APP_ADV_SLOW_DURATION;
    p_config->ble_adv_on_disconnect_disabled = false;
}

#ifndef MULTI_DEVICE_SWITCH
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event) {
    case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE: {
        // Prevent device from advertising on disconnect.
        ble_adv_modes_config_t config;
        advertising_config_get(&config);
        config.ble_adv_on_disconnect_disabled = true;
        ble_advertising_modes_config_set(&m_advertising, &config);

        // Disconnect all other bonded devices that currently are connected.
        // This is required to receive a service changed indication
        // on bootup after a successful (or aborted) Device Firmware Update.
        ble_conn_state_for_each_connected(disconnect, NULL);
        break;
    }

    case BLE_DFU_EVT_BOOTLOADER_ENTER:
        // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
        //           by delaying reset by reporting false in app_shutdown_handler
        break;

    case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
        // YOUR_JOB: Take corrective measures to resolve the issue
        //           like calling APP_ERROR_CHECK to reset the device.
        APP_ERROR_CHECK(false);
        break;

    case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
        // YOUR_JOB: Take corrective measures to resolve the issue
        //           like calling APP_ERROR_CHECK to reset the device.
        APP_ERROR_CHECK(false);
        break;

    default:
        break;
    }
}
#endif

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const* p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id) {
    case PM_EVT_CONN_SEC_SUCCEEDED:
        m_peer_id = p_evt->peer_id;
#ifdef MULTI_DEVICE_SWITCH        
        switch_device_update(m_peer_id);
#endif
        trig_event_param(USER_EVT_BLE_STATE_CHANGE, BLE_STATE_CONNECTED);
        break;

    case PM_EVT_BONDED_PEER_CONNECTED:
        trig_event_param(USER_EVT_BLE_STATE_CHANGE, BLE_STATE_CONNECTED);
        break;

    case PM_EVT_PEERS_DELETE_SUCCEEDED:
        advertising_start(false);
        break;

    case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        if (p_evt->params.peer_data_update_succeeded.flash_changed && (p_evt->params.peer_data_update_succeeded.data_id == PM_PEER_DATA_ID_BONDING)) {
            // Note: You should check on what kind of white list policy your application should use.

            whitelist_set(PM_PEER_ID_LIST_SKIP_NO_ID_ADDR);
        }
        break;

    case PM_EVT_CONN_SEC_CONFIG_REQ: {
        // allow pairing request from an already bonded peer.
        pm_conn_sec_config_t conn_sec_config = { .allow_repairing = true };
        pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);

        break;
    }

    default:
        break;
    }
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;

    set_device_name();

    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_KEYBOARD);
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Queued Write Module.
 */
static void qwr_init(void)
{
    ret_code_t err_code;
    nrf_ble_qwr_init_t qwr_init_obj = { 0 };

    qwr_init_obj.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init_obj);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing Device Information Service.
 */
static void dis_init(void)
{
    ret_code_t err_code;
    ble_dis_init_t dis_init_obj;
    ble_dis_pnp_id_t pnp_id;

    pnp_id.vendor_id_source = PNP_ID_VENDOR_ID_SOURCE;
    pnp_id.vendor_id = VENDOR_ID;
    pnp_id.product_id = PRODUCT_ID;
    pnp_id.product_version = DEVICE_VER;

    memset(&dis_init_obj, 0, sizeof(dis_init_obj));

    ble_srv_ascii_to_utf8(&dis_init_obj.manufact_name_str, MANUFACTURER_NAME);
    dis_init_obj.p_pnp_id = &pnp_id;

    dis_init_obj.dis_char_rd_sec = SEC_JUST_WORKS;

    err_code = ble_dis_init(&dis_init_obj);
    APP_ERROR_CHECK(err_code);
}

#ifndef MULTI_DEVICE_SWITCH
static void dfu_init(void)
{
    uint32_t err_code;
    ble_dfu_buttonless_init_t dfus_init = { 0 };

    dfus_init.evt_handler = ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
}
#endif

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = NULL;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt) {
    case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
        break;

    case BLE_ADV_EVT_DIRECTED:
        break;

    case BLE_ADV_EVT_FAST:
        break;

    case BLE_ADV_EVT_SLOW:
        break;

    case BLE_ADV_EVT_FAST_WHITELIST:
        break;

    case BLE_ADV_EVT_SLOW_WHITELIST:
        break;

    case BLE_ADV_EVT_IDLE:
        trig_event_param(USER_EVT_BLE_STATE_CHANGE, BLE_STATE_IDLE);
        break;

    case BLE_ADV_EVT_WHITELIST_REQUEST: {
        ble_gap_addr_t whitelist_addrs[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
        ble_gap_irk_t whitelist_irks[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
        uint32_t addr_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
        uint32_t irk_cnt = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;

        err_code = pm_whitelist_get(whitelist_addrs, &addr_cnt,
            whitelist_irks, &irk_cnt);
        APP_ERROR_CHECK(err_code);
        // Set the correct identities list (no excluding peers with no Central Address Resolution).
        identities_set(PM_PEER_ID_LIST_SKIP_NO_IRK);

        // Apply the whitelist.
        err_code = ble_advertising_whitelist_reply(&m_advertising,
            whitelist_addrs,
            addr_cnt,
            whitelist_irks,
            irk_cnt);
        APP_ERROR_CHECK(err_code);
    } break; //BLE_ADV_EVT_WHITELIST_REQUEST

    case BLE_ADV_EVT_PEER_ADDR_REQUEST: {
        pm_peer_data_bonding_t peer_bonding_data;

        // Only Give peer address if we have a handle to the bonded peer.
        if (m_peer_id != PM_PEER_ID_INVALID) {
            err_code = pm_peer_data_bonding_load(m_peer_id, &peer_bonding_data);
            if (err_code != NRF_ERROR_NOT_FOUND) {
                APP_ERROR_CHECK(err_code);

                // Manipulate identities to exclude peers with no Central Address Resolution.
                identities_set(PM_PEER_ID_LIST_SKIP_ALL);

                ble_gap_addr_t* p_peer_addr = &(peer_bonding_data.peer_ble_id.id_addr_info);
                err_code = ble_advertising_peer_addr_reply(&m_advertising, p_peer_addr);
                APP_ERROR_CHECK(err_code);
            }
        }
    } break; //BLE_ADV_EVT_PEER_ADDR_REQUEST

    default:
        break;
    }
}

#ifdef DYNAMIC_TX_POWER
//动态发射功率
static void ble_conn_handle_change(uint16_t old, uint16_t new)
{
    if (old != BLE_CONN_HANDLE_INVALID) {
        sd_ble_gap_rssi_stop(old);
    }
    if (new != BLE_CONN_HANDLE_INVALID) {
        sd_ble_gap_rssi_start(new, 5, 5);
    }
}

static uint8_t current_tx = 6; // 0dbm default.

static void ble_rssi_change(int8_t rssi)
{
    const int8_t tx_power_table[] = { -40, -20, -16, -12, -8, -4, 0, 3, 4 };
    if (rssi >= -65 && current_tx > 0)
        current_tx--;
    else if (rssi <= -80 && current_tx < sizeof(tx_power_table) - 1)
        current_tx++;
    else
        return;

    sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_CONN, m_conn_handle, tx_power_table[current_tx]);
}
#else
#define ble_conn_handle_change(A, B)
#define ble_rssi_change(A)
#endif

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const* p_ble_evt, void* p_context)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
        APP_ERROR_CHECK(err_code);
        ble_conn_handle_change(m_conn_handle, p_ble_evt->evt.gap_evt.conn_handle);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_conn_handle_change(m_conn_handle, BLE_CONN_HANDLE_INVALID);
        m_conn_handle = BLE_CONN_HANDLE_INVALID;
        trig_event_param(USER_EVT_BLE_STATE_CHANGE, BLE_STATE_DISCONNECT);
        break; // BLE_GAP_EVT_DISCONNECTED

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
        ble_gap_phys_t const phys = {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    } break;

    case BLE_GATTS_EVT_HVN_TX_COMPLETE:
        trig_event_param(USER_EVT_INTERNAL, INTERNAL_EVT_GATTS_TX_COMPLETE);
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
            BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
            BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GAP_EVT_AUTH_KEY_REQUEST:
        trig_event_param(USER_EVT_BLE_PASSKEY_STATE, PASSKEY_STATE_REQUIRE);
        break;

    case BLE_GAP_EVT_RSSI_CHANGED:
        ble_rssi_change(p_ble_evt->evt.gap_evt.params.rssi_changed.rssi);
        break;

    default:
        // No implementation needed.
        break;
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond = SEC_PARAM_BOND;
    sec_param.mitm = SEC_PARAM_MITM;
    sec_param.lesc = SEC_PARAM_LESC;
    sec_param.keypress = SEC_PARAM_KEYPRESS;
    sec_param.io_caps = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob = SEC_PARAM_OOB;
    sec_param.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc = 1;
    sec_param.kdist_own.id = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising errors.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void ble_advertising_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t err_code;
    uint8_t adv_flags;
    ble_advertising_init_t init;
#ifdef HIGH_TX_POWER
    int8_t tx_power_level = 4; //更改发射功率到+4dBm
#endif

    memset(&init, 0, sizeof(init));

    adv_flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = true;
    init.advdata.flags = adv_flags;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids = m_adv_uuids;
#ifdef HIGH_TX_POWER
    init.advdata.p_tx_power_level = &tx_power_level;
#endif

    advertising_config_get(&init.config);

    init.evt_handler = on_adv_evt;
    init.error_handler = ble_advertising_error_handler;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

void ble_passkey_send(uint8_t const* p_key)
{
    ret_code_t err_code = sd_ble_gap_auth_key_reply(
        m_conn_handle,
        BLE_GAP_AUTH_KEY_TYPE_PASSKEY,
        p_key);
    APP_ERROR_CHECK(err_code);
    trig_event_param(USER_EVT_BLE_PASSKEY_STATE, PASSKEY_STATE_SEND);
}

void ble_services_init()
{
    peer_manager_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    // services
    qwr_init();
    dis_init();
#ifndef MULTI_DEVICE_SWITCH
    dfu_init();
#endif
    conn_params_init();
}