/**
 * @file    dap_strings.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2019, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "descriptor.h"
#include <stdint.h>
#include <string.h>

/** Get Vendor ID string.
\param str Pointer to buffer to store the string.
\return String length.
*/
#define DAP_GetVendorString(str) (0U)

/** Get Product ID string.
\param str Pointer to buffer to store the string.
\return String length.
*/
#define DAP_GetProductString(str) (0U)

/** Get Serial Number string.
\param str Pointer to buffer to store the string.
\return String length.
*/
#define DAP_GetSerNumString(str) getSerial(str)

/** Get firmware version string.
\param str Pointer to buffer to store the string.
\return String length.
*/
uint8_t DAP_GetFirmwareVersionString(char* str)
{
    #define VER "0254"
    memcpy(str, VER, sizeof(VER));
    return sizeof(VER);
}
