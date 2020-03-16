/*
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
 * Copyright 2019, Cypress Semiconductor Corporation 
 * or a subsidiary of Cypress Semiconductor Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ----------------------------------------------------------------------
 *
 * $Date:        1. December 2017
 * $Revision:    V2.0.0
 *
 * Project:      CMSIS-DAP Source
 * Title:        DAP.c CMSIS-DAP Commands
 *
 *---------------------------------------------------------------------------*/

#include <string.h>
#include <stdbool.h>
#ifdef RTE_CMSIS_RTOS
#include "cmsis_os.h"
#endif
#include "DAP_config.h"
#include "DAP.h"
// #include "info.h"
#include "dap_strings.h"
#include "system.h"

#if (DAP_PACKET_SIZE < 64U)
#error "Minimum Packet Size is 64!"
#endif
#if (DAP_PACKET_SIZE > 32768U)
#error "Maximum Packet Size is 32768!"
#endif
#if (DAP_PACKET_COUNT < 1U)
#error "Minimum Packet Count is 1!"
#endif
#if (DAP_PACKET_COUNT > 255U)
#error "Maximum Packet Count is 255!"
#endif

// Clock Macros

#define MAX_SWJ_CLOCK(delay_cycles) \
    ((CPU_CLOCK / 2U) / (IO_PORT_WRITE_CYCLES + delay_cycles))

#define CLOCK_DELAY(swj_clock) \
    (((CPU_CLOCK / 2U) / swj_clock) - IO_PORT_WRITE_CYCLES)

__XDATA DAP_Data_t DAP_Data; // DAP Data
volatile uint8_t DAP_TransferAbort; // Transfer Abort Flag

static const uint8_t* request;
static uint8_t* response;

// static const char DAP_FW_Ver [] = DAP_FW_VER;

#if TARGET_DEVICE_FIXED
static const char TargetDeviceVendor[] = TARGET_DEVICE_VENDOR;
static const char TargetDeviceName[] = TARGET_DEVICE_NAME;
#endif

// Get DAP Information
//   id:      info identifier
//   info:    pointer to info data
//   return:  number of bytes in info data
static uint8_t DAP_Info(uint8_t id, uint8_t* info)
{
    uint8_t length = 0U;

    switch (id) {
    case DAP_ID_VENDOR:
        length = DAP_GetVendorString((char*)info);
        break;
    case DAP_ID_PRODUCT:
        length = DAP_GetProductString((char*)info);
        break;
    case DAP_ID_SER_NUM:
        length = DAP_GetSerNumString((char*)info);
        break;
    case DAP_ID_FW_VER: {
        // --- begin DAPLink change ---
        length = DAP_GetFirmwareVersionString((char*)info);
        // Original:
        //       memcpy(info, DAP_FW_Ver, sizeof(DAP_FW_Ver));
        //       length = (uint8_t)sizeof(DAP_FW_Ver);
        // --- end DAPLink change ---
        break;
    }
    case DAP_ID_DEVICE_VENDOR:
#if TARGET_DEVICE_FIXED
        length = (uint8_t)sizeof(TargetDeviceVendor);
        memcpy(info, TargetDeviceVendor, length);
#endif
        break;
    case DAP_ID_DEVICE_NAME:
#if TARGET_DEVICE_FIXED
        length = (uint8_t)sizeof(TargetDeviceName);
        memcpy(info, TargetDeviceName, length);
#endif
        break;
    case DAP_ID_CAPABILITIES:
        info[0] = ((DAP_SWD != 0) ? (1U << 0) : 0U)
            | ((DAP_JTAG != 0) ? (1U << 1) : 0U)
            | ((SWO_UART != 0) ? (1U << 2) : 0U)
            | ((SWO_MANCHESTER != 0) ? (1U << 3) : 0U)
            |
            /* Atomic Commands  */ (1U << 4)
            | ((TIMESTAMP_CLOCK != 0U) ? (1U << 5) : 0U)
            | ((SWO_STREAM != 0U) ? (1U << 6) : 0U);
        length = 1U;
        break;
    case DAP_ID_TIMESTAMP_CLOCK:
        break;
    case DAP_ID_SWO_BUFFER_SIZE:
        break;
    case DAP_ID_PACKET_SIZE:
        info[0] = (uint8_t)(DAP_PACKET_SIZE >> 0);
        info[1] = (uint8_t)(DAP_PACKET_SIZE >> 8);
        length = 2U;
        break;
    case DAP_ID_PACKET_COUNT:
        info[0] = DAP_PACKET_COUNT;
        length = 1U;
        break;
    default:
        break;
    }

    return (length);
}

// Process Delay command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_Delay()
{
    uint16_t delay;

    delay = (uint16_t)(*(request + 0)) | (uint16_t)(*(request + 1) << 8);
    DelayMs(delay);

    *response = DAP_OK;
    return (((uint16_t)2U << 8) | 1U);
}

// Process Host Status command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_HostStatus()
{
    switch (*request) {
    case DAP_DEBUGGER_CONNECTED:
        LED_CONNECTED_OUT((*(request + 1) & 1U));
        break;
    case DAP_TARGET_RUNNING:
        LED_RUNNING_OUT((*(request + 1) & 1U));
        break;
    default:
        *response = DAP_ERROR;
        return (((uint16_t)2U << 8) | 1U);
    }

    *response = DAP_OK;
    return (((uint16_t)2U << 8) | 1U);
}

// Process Connect command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_Connect()
{
    uint8_t port;

    if (*request == DAP_PORT_AUTODETECT) {
        port = DAP_DEFAULT_PORT;
    } else {
        port = *request;
    }

    switch (port) {
    case DAP_PORT_SWD:
        DAP_Data.debug_port = DAP_PORT_SWD;
        PORT_SWD_SETUP();
        break;
    default:
        port = DAP_PORT_DISABLED;
        break;
    }

    *response = port;
    return (((uint16_t)1U << 8) | 1U);
}

// Process Disconnect command and prepare response
//   response: pointer to response data
//   return:   number of bytes in response
static uint8_t DAP_Disconnect()
{
    DAP_Data.debug_port = DAP_PORT_DISABLED;
    PORT_OFF();

    *response = DAP_OK;
    return (1U);
}

// Process Reset Target command and prepare response
//   response: pointer to response data
//   return:   number of bytes in response
static uint8_t DAP_ResetTarget()
{
    *(response + 1) = RESET_TARGET();
    *(response + 0) = DAP_OK;
    return (2U);
}

// Process SWJ Pins command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_SWJ_Pins()
{
    uint8_t value;
    uint8_t select;
    uint8_t wait;

    value = *(request + 0);
    select = *(request + 1);
    wait = (*(request + 2)) | (*(request + 3)) | (*(request + 4)) | (*(request + 5));

    if ((select & (1U << DAP_SWJ_SWCLK_TCK)) != 0U) {
        if ((value & (1U << DAP_SWJ_SWCLK_TCK)) != 0U) {
            PIN_SWCLK_TCK_SET();
        } else {
            PIN_SWCLK_TCK_CLR();
        }
    }
    if ((select & (1U << DAP_SWJ_SWDIO_TMS)) != 0U) {
        if ((value & (1U << DAP_SWJ_SWDIO_TMS)) != 0U) {
            PIN_SWDIO_TMS_SET();
        } else {
            PIN_SWDIO_TMS_CLR();
        }
    }
    if ((select & (1U << DAP_SWJ_TDI)) != 0U) {
        PIN_TDI_OUT(value >> DAP_SWJ_TDI);
    }
    if ((select & (1U << DAP_SWJ_nTRST)) != 0U) {
        PIN_nTRST_OUT(value >> DAP_SWJ_nTRST);
    }
    if ((select & (1U << DAP_SWJ_nRESET)) != 0U) {
        PIN_nRESET_OUT(value >> DAP_SWJ_nRESET);
    }

    if (wait != 0U) {
        do {
            if ((select & (1U << DAP_SWJ_SWCLK_TCK)) != 0U) {
                if ((value >> DAP_SWJ_SWCLK_TCK) ^ PIN_SWCLK_TCK_IN()) {
                    continue;
                }
            }
            if ((select & (1U << DAP_SWJ_SWDIO_TMS)) != 0U) {
                if ((value >> DAP_SWJ_SWDIO_TMS) ^ PIN_SWDIO_TMS_IN()) {
                    continue;
                }
            }
            if ((select & (1U << DAP_SWJ_TDI)) != 0U) {
                if ((value >> DAP_SWJ_TDI) ^ PIN_TDI_IN()) {
                    continue;
                }
            }
            if ((select & (1U << DAP_SWJ_nTRST)) != 0U) {
                if ((value >> DAP_SWJ_nTRST) ^ PIN_nTRST_IN()) {
                    continue;
                }
            }
            if ((select & (1U << DAP_SWJ_nRESET)) != 0U) {
                if ((value >> DAP_SWJ_nRESET) ^ PIN_nRESET_IN()) {
                    continue;
                }
            }
            break;
        } while (1);
    }

    value = (PIN_SWCLK_TCK_IN() << DAP_SWJ_SWCLK_TCK) | (PIN_SWDIO_TMS_IN() << DAP_SWJ_SWDIO_TMS) | (PIN_TDI_IN() << DAP_SWJ_TDI) | (PIN_TDO_IN() << DAP_SWJ_TDO) | (PIN_nTRST_IN() << DAP_SWJ_nTRST) | (PIN_nRESET_IN() << DAP_SWJ_nRESET);

    *response = (uint8_t)value;

    return (((uint16_t)6U << 8) | 1U);
}

// Process SWJ Clock command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_SWJ_Clock()
{
    (void)request;
    *response = DAP_OK;
    return (((uint16_t)4U << 8) | 1U);
}

// Process SWJ Sequence command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_SWJ_Sequence()
{
    uint16_t count;

    count = *request++;
    if (count == 0U) {
        count = 256U;
    }

    SWJ_Sequence(count, request);
    *response = DAP_OK;

    count = (count + 7U) >> 3;

    return (((uint16_t)(count + 1U) << 8) | 1U);
}

// Process SWD Configure command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_SWD_Configure()
{
    uint8_t value;

    value = *request;
    DAP_Data.swd_conf.turnaround = (value & 0x03U) + 1U;
    DAP_Data.swd_conf.data_phase = (value & 0x04U) ? 1U : 0U;

    *response = DAP_OK;

    return (((uint16_t)1U << 8) | 1U);
}

// Process SWD Sequence command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_SWD_Sequence()
{
    uint8_t sequence_info;
    uint8_t sequence_count;
    uint8_t request_count;
    uint8_t response_count;
    uint8_t count;

    *response++ = DAP_OK;
    request_count = 1U;
    response_count = 1U;

    sequence_count = *request++;
    while (sequence_count--) {
        sequence_info = *request++;
        count = sequence_info & SWD_SEQUENCE_CLK;
        if (count == 0U) {
            count = 64U;
        }
        count = (count + 7U) / 8U;
        if ((sequence_info & SWD_SEQUENCE_DIN) != 0U) {
            PIN_SWDIO_OUT_DISABLE();
        } else {
            PIN_SWDIO_OUT_ENABLE();
        }
        SWD_Sequence(sequence_info, request, response);
        if (sequence_count == 0U) {
            PIN_SWDIO_OUT_ENABLE();
        }
        if ((sequence_info & SWD_SEQUENCE_DIN) != 0U) {
            request_count++;
            response += count;
            response_count += count;
        } else {
            request += count;
            request_count += count + 1U;
        }
    }

    return (((uint16_t)request_count << 8) | response_count);
}

// Process JTAG Sequence command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_JTAG_Sequence()
{
    uint8_t sequence_info;
    uint8_t sequence_count;
    uint8_t request_count;
    uint8_t response_count;
    uint8_t count;

    *response++ = DAP_ERROR;
    request_count = 1U;
    response_count = 1U;

    sequence_count = *request++;
    while (sequence_count--) {
        sequence_info = *request++;
        count = sequence_info & JTAG_SEQUENCE_TCK;
        if (count == 0U) {
            count = 64U;
        }
        count = (count + 7U) / 8U;
        request += count;
        request_count += count + 1U;
    }

    return (((uint16_t)request_count << 8) | response_count);
}

// Process JTAG Configure command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_JTAG_Configure()
{
    uint8_t count;
    count = *request;
    *response = DAP_ERROR;

    return ((((uint16_t)count + 1U) << 8) | 1U);
}

// Process JTAG IDCODE command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_JTAG_IDCode()
{
    (void)request;
    *response = DAP_ERROR;
    return (((uint16_t)1U << 8) | 1U);
}

// Process Transfer Configure command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_TransferConfigure()
{
    DAP_Data.transfer.idle_cycles = *(request + 0);
    DAP_Data.transfer.retry_count = (uint16_t) * (request + 1) | (uint16_t)(*(request + 2) << 8);
    DAP_Data.transfer.match_retry = (uint16_t) * (request + 3) | (uint16_t)(*(request + 4) << 8);

    *response = DAP_OK;
    return (((uint16_t)5U << 8) | 1U);
}

// Process SWD Transfer command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_SWD_Transfer()
{
    const uint8_t* request_head;
    uint8_t request_count;
    uint8_t request_value;
    uint8_t* response_head;
    uint8_t response_count;
    uint8_t response_value;
    bool post_read;
    uint8_t check_write;
    uint8_t match_value[4];
    uint16_t match_retry;
    uint16_t retry;
    uint8_t data[4];

    request_head = request;

    response_count = 0U;
    response_value = 0U;
    response_head = response;
    response += 2;

    DAP_TransferAbort = 0U;

    post_read = 0U;
    check_write = 0U;

    const uint8_t* req = request;
    req++; // Ignore DAP index

    request_count = *req++;

    for (; request_count != 0U; request_count--) {
        request_value = *req++;
        if ((request_value & DAP_TRANSFER_RnW) != 0U) {
            // Read register
            if (post_read) {
                // Read was posted before
                retry = DAP_Data.transfer.retry_count;
                if ((request_value & (DAP_TRANSFER_APnDP | DAP_TRANSFER_MATCH_VALUE)) == DAP_TRANSFER_APnDP) {
                    // Read previous AP data and post next AP read
                    do {
                        response_value = SWD_Transfer(request_value, data);
                    } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                } else {
                    // Read previous AP data
                    do {
                        response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, data);
                    } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                    post_read = 0U;
                }
                if (response_value != DAP_TRANSFER_OK) {
                    break;
                }
                // Store previous AP data
                for (uint8_t i = 0; i < 4; i++) {
                    *response++ = data[i];
                }
            }
            if ((request_value & DAP_TRANSFER_MATCH_VALUE) != 0U) {
                // Read with value match
                for (uint8_t i = 0; i < 4; i++)
                    match_value[i] = *(req++);

                match_retry = DAP_Data.transfer.match_retry;
                if ((request_value & DAP_TRANSFER_APnDP) != 0U) {
                    // Post AP read
                    retry = DAP_Data.transfer.retry_count;
                    do {
                        response_value = SWD_Transfer(request_value, NULL);
                    } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                    if (response_value != DAP_TRANSFER_OK) {
                        break;
                    }
                }
                bool mismatch = false;
                do {
                    // Read register until its value matches or retry counter expires
                    retry = DAP_Data.transfer.retry_count;
                    do {
                        response_value = SWD_Transfer(request_value, data);
                    } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                    if (response_value != DAP_TRANSFER_OK) {
                        break;
                    }
                    mismatch = ((data[0] & DAP_Data.transfer.match_mask[0]) != match_value[0]
                        || (data[1] & DAP_Data.transfer.match_mask[1]) != match_value[1]
                        || (data[2] & DAP_Data.transfer.match_mask[2]) != match_value[2]
                        || (data[3] & DAP_Data.transfer.match_mask[3]) != match_value[3]);
                } while (mismatch && match_retry-- && !DAP_TransferAbort);
                if (mismatch) {
                    response_value |= DAP_TRANSFER_MISMATCH;
                }
                if (response_value != DAP_TRANSFER_OK) {
                    break;
                }
            } else {
                // Normal read
                retry = DAP_Data.transfer.retry_count;
                if ((request_value & DAP_TRANSFER_APnDP) != 0U) {
                    // Read AP register
                    if (post_read == 0U) {
                        // Post AP read
                        do {
                            response_value = SWD_Transfer(request_value, NULL);
                        } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                        if (response_value != DAP_TRANSFER_OK) {
                            break;
                        }
                        post_read = 1U;
                    }
                } else {
                    // Read DP register
                    do {
                        response_value = SWD_Transfer(request_value, data);
                    } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                    if (response_value != DAP_TRANSFER_OK) {
                        break;
                    }
                    // Store data
                    for (uint8_t i = 0; i < 4; i++) {
                        *response++ = data[i];
                    }
                }
            }
            check_write = 0U;
        } else {
            // Write register
            if (post_read) {
                // Read previous data
                retry = DAP_Data.transfer.retry_count;
                do {
                    response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, data);
                } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                if (response_value != DAP_TRANSFER_OK) {
                    break;
                }
                // Store previous data
                for (uint8_t i = 0; i < 4; i++) {
                    *response++ = data[i];
                }
                post_read = 0U;
            }
            // Load data
            for (uint8_t i = 0; i < 4; i++) {
                data[i] = *req++;
            }
            if ((request_value & DAP_TRANSFER_MATCH_MASK) != 0U) {
                // Write match mask
                for (uint8_t i = 0; i < 4; i++)
                    DAP_Data.transfer.match_mask[i] = data[i];
                response_value = DAP_TRANSFER_OK;
            } else {
                // Write DP/AP register
                retry = DAP_Data.transfer.retry_count;
                do {
                    response_value = SWD_Transfer(request_value, data);
                } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
                if (response_value != DAP_TRANSFER_OK) {
                    break;
                }
                check_write = 1U;
            }
        }
        response_count++;
        if (DAP_TransferAbort) {
            break;
        }
    }

    for (; request_count != 0U; request_count--) {
        // Process canceled requests
        request_value = *req++;
        if ((request_value & DAP_TRANSFER_RnW) != 0U) {
            // Read register
            if ((request_value & DAP_TRANSFER_MATCH_VALUE) != 0U) {
                // Read with value match
                req += 4;
            }
        } else {
            // Write register
            req += 4;
        }
    }

    if (response_value == DAP_TRANSFER_OK) {
        if (post_read) {
            // Read previous data
            retry = DAP_Data.transfer.retry_count;
            do {
                response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, data);
            } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
            if (response_value != DAP_TRANSFER_OK) {
                goto end;
            }
            // Store previous data
            for (uint8_t i = 0; i < 4; i++) {
                *response++ = data[i];
            }
        } else if (check_write) {
            // Check last write
            retry = DAP_Data.transfer.retry_count;
            do {
                response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, NULL);
            } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
        }
    }

end:
    *(response_head + 0) = (uint8_t)response_count;
    *(response_head + 1) = (uint8_t)response_value;

    return (((uint16_t)(request - request_head) << 8) | (uint16_t)(response - response_head));
}

// Process Dummy Transfer command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_Dummy_Transfer()
{
    const uint8_t* request_head;
    uint8_t request_count;
    uint8_t request_value;

    request_head = request;
    const uint8_t* req = request;

    req++; // Ignore DAP index

    request_count = *req++;

    for (; request_count != 0U; request_count--) {
        // Process dummy requests
        request_value = *req++;
        if ((request_value & DAP_TRANSFER_RnW) != 0U) {
            // Read register
            if ((request_value & DAP_TRANSFER_MATCH_VALUE) != 0U) {
                // Read with value match
                req += 4;
            }
        } else {
            // Write register
            req += 4;
        }
    }

    *(response + 0) = 0U; // Response count
    *(response + 1) = 0U; // Response value

    return (((uint16_t)(req - request_head) << 8) | 2U);
}

// Process Transfer command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_Transfer()
{
    uint16_t num;

    switch (DAP_Data.debug_port) {
    case DAP_PORT_SWD:
        num = DAP_SWD_Transfer();
        break;
    default:
        num = DAP_Dummy_Transfer();
        break;
    }

    return (num);
}

// Process SWD Transfer Block command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response
static uint8_t DAP_SWD_TransferBlock()
{
    uint16_t request_count;
    uint8_t request_value;
    uint16_t response_count;
    uint8_t response_value;
    uint8_t* response_head;
    uint16_t retry;
    uint8_t data[4];

    response_count = 0U;
    response_value = 0U;
    response_head = response;
    response += 3;

    DAP_TransferAbort = 0U;

    const uint8_t* req = request;
    req++; // Ignore DAP index

    request_count = (uint16_t)(*(req + 0) << 0) | (uint16_t)(*(req + 1) << 8);
    req += 2;
    if (request_count == 0U) {
        goto end;
    }

    request_value = *req++;
    if ((request_value & DAP_TRANSFER_RnW) != 0U) {
        // Read register block
        if ((request_value & DAP_TRANSFER_APnDP) != 0U) {
            // Post AP read
            retry = DAP_Data.transfer.retry_count;
            do {
                response_value = SWD_Transfer(request_value, NULL);
            } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
            if (response_value != DAP_TRANSFER_OK) {
                goto end;
            }
        }
        while (request_count--) {
            // Read DP/AP register
            if ((request_count == 0U) && ((request_value & DAP_TRANSFER_APnDP) != 0U)) {
                // Last AP read
                request_value = DP_RDBUFF | DAP_TRANSFER_RnW;
            }
            retry = DAP_Data.transfer.retry_count;
            do {
                response_value = SWD_Transfer(request_value, data);
            } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
            if (response_value != DAP_TRANSFER_OK) {
                goto end;
            }
            // Store data
            for (uint8_t i = 0; i < 4; i++) {
                *response++ = data[i];
            }
            response_count++;
        }
    } else {
        // Write register block
        while (request_count--) {
            // Load data
            for (uint8_t i = 0; i < 4; i++) {
                data[i] = *req++;
            }

            // Write DP/AP register
            retry = DAP_Data.transfer.retry_count;
            do {
                response_value = SWD_Transfer(request_value, data);
            } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
            if (response_value != DAP_TRANSFER_OK) {
                goto end;
            }
            response_count++;
        }
        // Check last write
        retry = DAP_Data.transfer.retry_count;
        do {
            response_value = SWD_Transfer(DP_RDBUFF | DAP_TRANSFER_RnW, NULL);
        } while ((response_value == DAP_TRANSFER_WAIT) && retry-- && !DAP_TransferAbort);
    }

end:
    *(response_head + 0) = (uint8_t)(response_count >> 0);
    *(response_head + 1) = (uint8_t)(response_count >> 8);
    *(response_head + 2) = (uint8_t)response_value;

    return ((uint8_t)(response - response_head));
}

// Process Transfer Block command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_TransferBlock()
{
    uint8_t num;

    switch (DAP_Data.debug_port) {
    case DAP_PORT_SWD:
        num = DAP_SWD_TransferBlock();
        break;
    default:
        *(response + 0) = 0U; // Response count [7:0]
        *(response + 1) = 0U; // Response count[15:8]
        *(response + 2) = 0U; // Response value
        num = 3U;
        break;
    }

    if ((*(request + 3) & DAP_TRANSFER_RnW) != 0U) {
        // Read register block
        return ((uint16_t)4U << 8) | num;
    } else {
        // Write register block
        return ((4U + (((uint16_t)(*(request + 1)) | (uint16_t)(*(request + 2) << 8)) * 4)) << 8) | num;
    }
}

// Process SWD Write ABORT command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response
static uint8_t DAP_SWD_WriteAbort()
{
    // Write Abort register
    SWD_Transfer(DP_ABORT, (uint8_t*)&request[1]);

    *response = DAP_OK;
    return (1U);
}

// Process Write ABORT command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
static uint16_t DAP_WriteAbort()
{
    uint8_t num;

    switch (DAP_Data.debug_port) {
    case DAP_PORT_SWD:
        num = DAP_SWD_WriteAbort();
        break;
    default:
        *response = DAP_ERROR;
        num = 1U;
        break;
    }
    return (((uint16_t)5U << 8) | num);
}

// Process DAP Vendor command request and prepare response
// Default function (can be overridden)
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
uint16_t DAP_ProcessVendorCommand()
{
    (void)request;
    *response = ID_DAP_Invalid;
    return (((uint16_t)1U << 8) | 1U);
}

// Process DAP Vendor extended command request and prepare response
// Default function (can be overridden)
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
uint16_t DAP_ProcessVendorCommandEx()
{
    (void)request;
    *response = ID_DAP_Invalid;
    return (((uint16_t)1U << 8) | 1U);
}

// Process DAP command request and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
uint16_t DAP_ProcessCommand(const uint8_t* req, uint8_t* resp)
{
    uint16_t num;
    request = req;
    response = resp;

    if ((*request >= ID_DAP_Vendor0) && (*request <= ID_DAP_Vendor31)) {
        return DAP_ProcessVendorCommand();
    }

    if ((*request >= ID_DAP_VendorExFirst) && (*request <= ID_DAP_VendorExLast)) {
        return DAP_ProcessVendorCommandEx();
    }

    *response++ = *request;

    switch (*request++) {
    case ID_DAP_Info:
        num = DAP_Info(*request, response + 1);
        *response = (uint8_t)num;
        return (((uint16_t)2U << 8) + 2U + num);

    case ID_DAP_HostStatus:
        num = DAP_HostStatus();
        break;

    case ID_DAP_Connect:
        num = DAP_Connect();
        break;
    case ID_DAP_Disconnect:
        num = DAP_Disconnect();
        break;

    case ID_DAP_Delay:
        num = DAP_Delay();
        break;

    case ID_DAP_ResetTarget:
        num = DAP_ResetTarget();
        break;

    case ID_DAP_SWJ_Pins:
        num = DAP_SWJ_Pins();
        break;
    case ID_DAP_SWJ_Clock:
        num = DAP_SWJ_Clock();
        break;
    case ID_DAP_SWJ_Sequence:
        num = DAP_SWJ_Sequence();
        break;

    case ID_DAP_SWD_Configure:
        num = DAP_SWD_Configure();
        break;
    case ID_DAP_SWD_Sequence:
        num = DAP_SWD_Sequence();
        break;

    case ID_DAP_JTAG_Sequence:
        num = DAP_JTAG_Sequence();
        break;
    case ID_DAP_JTAG_Configure:
        num = DAP_JTAG_Configure();
        break;
    case ID_DAP_JTAG_IDCODE:
        num = DAP_JTAG_IDCode();
        break;

    case ID_DAP_TransferConfigure:
        num = DAP_TransferConfigure();
        break;
    case ID_DAP_Transfer:
        num = DAP_Transfer();
        break;
    case ID_DAP_TransferBlock:
        num = DAP_TransferBlock();
        break;

    case ID_DAP_WriteABORT:
        num = DAP_WriteAbort();
        break;

    default:
        *(response - 1) = ID_DAP_Invalid;
        return (((uint16_t)1U << 8) | 1U);
    }

    return (((uint16_t)1U << 8) + 1U + num);
}

// Execute DAP command (process request and prepare response)
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response (lower 8 bits)
//             number of bytes in request (upper 8 bits)
uint16_t DAP_ExecuteCommand(const uint8_t* req, uint8_t* resp)
{
    uint16_t num, n;
    uint8_t cnt;

    if (*req == ID_DAP_ExecuteCommands) {
        *resp++ = *req++;
        cnt = *req++;
        *resp++ = (uint8_t)cnt;
        num = ((uint16_t)2U << 8) | 2U;
        while (cnt--) {
            n = DAP_ProcessCommand(req, resp);
            num += n;
            req += (uint8_t)(n >> 8);
            resp += (uint8_t)n;
        }
        return (num);
    }

    return DAP_ProcessCommand(req, resp);
}

// Setup DAP
void DAP_Setup(void)
{
    // Default settings
    DAP_Data.debug_port = 0U;
    DAP_Data.clock_delay = CLOCK_DELAY(DAP_DEFAULT_SWJ_CLOCK);
    DAP_Data.transfer.idle_cycles = 0U;
    DAP_Data.transfer.retry_count = 100U;
    DAP_Data.transfer.match_retry = 0U;
    DAP_Data.transfer.match_mask[0] = 0;
    DAP_Data.transfer.match_mask[1] = 0;
    DAP_Data.transfer.match_mask[2] = 0;
    DAP_Data.transfer.match_mask[3] = 0;
    DAP_Data.swd_conf.turnaround = 1U;
    DAP_Data.swd_conf.data_phase = 0U;

    DAP_SETUP(); // Device specific setup
    PORT_OFF(); // Avoid influence other device.
}
