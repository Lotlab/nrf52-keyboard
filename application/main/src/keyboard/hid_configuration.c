#include "hid_configuration.h"
#include "usb_comm.h"
#include <string.h>

#define HID_PROTOCOL 4
#define APP_VERSION 1,3,1,1

static void send_information()
{
    const uint8_t info[] = {
        CONF_VENDOR_ID >> 8,
        CONF_VENDOR_ID & 0xFF,
        CONF_PRODUCT_ID >> 8,
        CONF_PRODUCT_ID & 0xFF,
        DEVICE_VER & 0xFF,
        HID_PROTOCOL,
        APP_VERSION,
    };
}

void hid_on_recv(uint8_t command, uint8_t len, uint8_t* data)
{
    if (command >= 0x80) {

    } else {
        switch (command) {
        case HID_CMD_GET_INFORMATION:
            if (len != 0)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            else
                send_information();
            break;
        case HID_CMD_GET_SINGLE_KEY:
            if (len != 3)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_GET_SINGLE_FN:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_GET_ALL_KEYS:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_GET_ALL_FNS:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_GET_SINGLE_CONFIG:
            if (len != 2)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_GET_ALL_CONFIG:
            if (len != 0)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_GET_ALL_MACRO:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_SET_SINGLE_KEY:
            if (len != 5)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_SET_SINGLE_FN:
            if (len != 3)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_SET_ALL_KEYS:
            break;
        case HID_CMD_SET_ALL_FNS:
            break;
        case HID_CMD_SET_SINGLE_CONFIG:
            break;
        case HID_CMD_SET_ALL_CONFIG:
            break;
        case HID_CMD_SET_ALL_MACRO:
            break;
        case HID_CMD_WRITE_CONFIG:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        case HID_CMD_RESET_CONFIG:
            if (len != 1)
                hid_response_generic(HID_RESP_PARAMETER_ERROR);
            break;
        default:
            hid_response_generic(HID_RESP_UNDEFINED);
            break;
        }
    }
}

/**
 * 响应HID数据
 */
static void hid_response_success(uint8_t len, uint8_t* data)
{
    uint8_t buff[63];
    buff[0] = 0x00;
    buff[1] = len;
    memcpy(&buff[2], data, len);
    uart_send_raw(len + 2, buff);
}

/**
 * 通用HID响应
 */
static void hid_response_generic(enum hid_response response)
{
    uart_send_raw(1, &response);
}
