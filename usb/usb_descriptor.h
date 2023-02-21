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

#pragma once

#include <stdint.h>
#include "config.h"

#define USB_VID               VENDOR_ID     // Vendor ID (VID)
#define USB_PID               PRODUCT_ID    // Product ID (PID)
#define VER_FW_H              ((DEVICE_VER & 0xFF00) >> 8) // Device release number, in binary-coded decimal
#define VER_FW_L              (DEVICE_VER & 0xFF) // Device release number, in binary-coded decimal

#define EP0_PACKET_SIZE       0x08
#define NKRO_REPORT_KEYS      (NKRO_EPSIZE - 1)
#define MAX_PACKET_SIZE       64

enum StringDescriptor {
    STRING_DESCRIPTOR_LANG,
    STRING_DESCRIPTOR_MANUFACTURER,
    STRING_DESCRIPTOR_DEVICE,
    STRING_DESCRIPTOR_SERIAL,
    STRING_DESCRIPTOR_INTERFACE_0,
    STRING_DESCRIPTOR_INTERFACE_1,
    STRING_DESCRIPTOR_INTERFACE_2,
    STRING_DESCRIPTOR_INTERFACE_3,
    STRING_DESCRIPTOR_INTERFACE_END,
};


uint8_t const DeviceDescriptor[] = {
    sizeof(DeviceDescriptor),               // Length of this descriptor
    0x01,                                   // Type code of this descriptor
    0x00, 0x02,                             // Release of USB spec
    0x00,                                   // Device's base class code
    0x00,                                   // Device's sub class code
    0x00,                                   // Device's protocol type code
    EP0_PACKET_SIZE,                        // End point 0's packet size
    USB_VID&0xFF, USB_VID>>8,               // Vendor ID for device
    USB_PID&0xFF, USB_PID>>8,               // Product ID for device
    VER_FW_L, VER_FW_H,                     // Revision level of device
    STRING_DESCRIPTOR_MANUFACTURER,         // Index of manufacturer name string desc
    STRING_DESCRIPTOR_DEVICE,               // Index of product name string desc
    STRING_DESCRIPTOR_SERIAL,               // Index of serial number string desc
    1                                       //  Number of configurations supported
};

#define DESCRIPTOR_TOTAL_LENGTH             sizeof(ConfigDescriptor)
#ifdef ONBOARD_CMSIS_DAP
#define USB_NUM_INTERFACES                  0x04
#else
#define USB_NUM_INTERFACES                  0x03
#endif
#define USB_SUPPORT_REM_WAKE                0x20    // 0x20 support, 0x00 not support
#define USB_SUPPORT_SELF_POWERED            0x80    // not self-powered
#define USB_MAX_POWER                       0xfa    // 500 mA

#define report_desc_size_HID0               sizeof(report_desc_HID0)
#define report_desc_size_HID1               sizeof(report_desc_HID1)
#define report_desc_size_HID2               sizeof(report_desc_HID2)
#define report_desc_size_HID3               sizeof(report_desc_HID3)

#define HID0_REPORT_INTERFACE              0              // Report interface number of HID0
#define HID0_OUTEP_ADDR                    0x01           // Output Endpoint number of HID0
#define HID0_INEP_ADDR                     0x81           // Input Endpoint number of HID0

#define HID1_REPORT_INTERFACE              1              // Report interface number of HID1
#define HID1_OUTEP_ADDR                    0x02           // Output Endpoint number of HID1
#define HID1_INEP_ADDR                     0x82           // Input Endpoint number of HID1

#define HID2_REPORT_INTERFACE              2              // Report interface number of HID2
#define HID2_OUTEP_ADDR                    0x03           // Output Endpoint number of HID2
#define HID2_INEP_ADDR                     0x83           // Input Endpoint number of HID2

#define HID3_REPORT_INTERFACE              3              // Report interface number of HID3
#define HID3_OUTEP_ADDR                    0x04           // Output Endpoint number of HID3
#define HID3_INEP_ADDR                     0x84           // Input Endpoint number of HID3


/*-----------------------------------------------------------------------------+
| Configuration Descriptor                                                     |
|-----------------------------------------------------------------------------*/
uint8_t const ConfigDescriptor []=
{
    /* Generic part */

    // CONFIGURATION DESCRIPTOR (9 bytes)
    0x09,                                              // bLength
    0x02,                                              // bDescriptorType
    DESCRIPTOR_TOTAL_LENGTH, 0x00,                     // wTotalLength
    USB_NUM_INTERFACES,                                // bNumInterfaces
    0x01,                                              // bConfigurationvalue
    0x00,                                              // iConfiguration Description offset
    USB_SUPPORT_SELF_POWERED | USB_SUPPORT_REM_WAKE,   // bmAttributes, bus power, remote wakeup
    USB_MAX_POWER,                                     // Max. Power Consumption

    /******************************************************* start of HID*************************************/

    /*start HID[0] Here */

    //-------- Descriptor for HID class device -------------------------------------
    // INTERFACE DESCRIPTOR (9 bytes)
    0x09,                               // bLength
    0x04,                               // bDescriptorType: 4
    0x00,                               // bInterfaceNumber: 0
    0x00,                               // bAlternateSetting
    2,                                  // bNumEndpoints
    0x03,                               // bInterfaceClass: 3 = HID Device
    1,                                  // bInterfaceSubClass:
    1,                                  // bInterfaceProtocol:
    STRING_DESCRIPTOR_INTERFACE_0,      // iInterface:

    // HID DESCRIPTOR (9 bytes)
    0x09,                                 // bLength of HID descriptor
    0x21,                                 // HID Descriptor Type: 0x21
    0x01,0x01,                            // HID Revision number 1.01
    0x00,                                // Target country, nothing specified (00h)
    0x01,                                // Number of HID classes to follow
    0x22,                                // Report descriptor type
    (report_desc_size_HID0 & 0x0ff),     // Total length of report descriptor
    (report_desc_size_HID0 > 0xff ? report_desc_size_HID0 >> 8 : 0x00),

    0x07,                               // bLength of EndPoint Descriptor
    0x05,                               // bDescriptorType of ENDPOINT
    HID0_INEP_ADDR,                     // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID0_OUTEP_ADDR,                    // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms
    /* end of HID[0]*/

    //-------- Descriptor for HID class device -------------------------------------
    // INTERFACE DESCRIPTOR (9 bytes)
    0x09,                               // bLength
    0x04,                               // bDescriptorType: 4
    0x01,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    1,                                  // bNumEndpoints
    0x03,                               // bInterfaceClass: 3 = HID Device
    0,                                  // bInterfaceSubClass:
    0,                                  // bInterfaceProtocol:
    STRING_DESCRIPTOR_INTERFACE_1,      // iInterface:

    // HID DESCRIPTOR (9 bytes)
    0x09,                                 // bLength of HID descriptor
    0x21,                                 // HID Descriptor Type: 0x21
    0x01,0x01,                            // HID Revision number 1.01
    0x00,                                // Target country, nothing specified (00h)
    0x01,                                // Number of HID classes to follow
    0x22,                                // Report descriptor type
    (report_desc_size_HID1 & 0x0ff),  // Total length of report descriptor
    (report_desc_size_HID1 > 0xFF ? report_desc_size_HID1 >> 8 : 0x00),

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID1_INEP_ADDR,                     // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms


    //-------- Descriptor for HID class device -------------------------------------
    // INTERFACE DESCRIPTOR (9 bytes)
    0x09,                               // bLength
    0x04,                               // bDescriptorType: 4
    0x02,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    2,                                  // bNumEndpoints
    0x03,                               // bInterfaceClass: 3 = HID Device
    0,                                  // bInterfaceSubClass:
    0,                                  // bInterfaceProtocol:
    STRING_DESCRIPTOR_INTERFACE_2,      // iInterface:

    // HID DESCRIPTOR (9 bytes)
    0x09,                                 // bLength of HID descriptor
    0x21,                                 // HID Descriptor Type: 0x21
    0x01,0x01,                            // HID Revision number 1.01
    0x00,                                // Target country, nothing specified (00h)
    0x01,                                // Number of HID classes to follow
    0x22,                                // Report descriptor type
    (report_desc_size_HID2 & 0x0ff),  // Total length of report descriptor
    (report_desc_size_HID2 > 0xFF ? report_desc_size_HID2 >> 8 : 0x00),

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID2_INEP_ADDR,                     // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID2_OUTEP_ADDR,                    // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms

#ifdef ONBOARD_CMSIS_DAP
    //-------- Descriptor for HID class device -------------------------------------
    // INTERFACE DESCRIPTOR (9 bytes)
    0x09,                               // bLength
    0x04,                               // bDescriptorType: 4
    0x03,                               // bInterfaceNumber
    0x00,                               // bAlternateSetting
    2,                                  // bNumEndpoints
    0x03,                               // bInterfaceClass: 3 = HID Device
    0,                                  // bInterfaceSubClass:
    0,                                  // bInterfaceProtocol:
    STRING_DESCRIPTOR_INTERFACE_3,      // iInterface:1

    // HID DESCRIPTOR (9 bytes)
    0x09,                                 // bLength of HID descriptor
    0x21,                                 // HID Descriptor Type: 0x21
    0x01,0x01,                            // HID Revision number 1.01
    0x00,                                // Target country, nothing specified (00h)
    0x01,                                // Number of HID classes to follow
    0x22,                                // Report descriptor type
    (report_desc_size_HID3 & 0x0ff),  // Total length of report descriptor
    (report_desc_size_HID3 > 0xFF ? report_desc_size_HID3 >> 8 : 0x00),

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID3_INEP_ADDR,                     // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID3_OUTEP_ADDR,                    // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    1,                                  // bInterval, ms
    /******************************************************* end of HID**************************************/
#endif
};

/*-----------------------------------------------------------------------------+
| String Descriptor                                                            |
|-----------------------------------------------------------------------------*/
const uint8_t LangStringDesc[] = {
    // String index0, language support
    4,        // Length of language descriptor ID
    3,        // LANGID tag
    0x09, 0x04,    // 0x0409 for English
};

const uint8_t InterfaceStringDesc[] = {
    // String index5, Interface String
    26,        // Length of this string descriptor
    3,        // bDescriptorType
    'H',0x00,'I',0x00,'D',0x00,' ',0x00,'K',0x00,'e',0x00,
    'y',0x00,'b',0x00,'o',0x00,'a',0x00,'r',0x00,'d',0x00,

    // String index6, Interface String
    36,        // Length of this string descriptor
    3,        // bDescriptorType
    'E',0x00,'x',0x00,'t',0x00,'r',0x00,'a',0x00,' ',0x00,
    'K',0x00,'e',0x00,'y',0x00,' ',0x00,'S',0x00,'u',0x00,
    'p',0x00,'p',0x00,'o',0x00,'r',0x00,'t',0x00,

    // String index7, Interface String
    40,        // Length of this string descriptor
    3,        // bDescriptorType
    'L',0x00,'o',0x00,'t',0x00,'l',0x00,'a',0x00,'b',0x00,
    ' ',0x00,'C',0x00,'o',0x00,'n',0x00,'f',0x00,'i',0x00,
    'g',0x00,'u',0x00,'r',0x00,'a',0x00,'t',0x00,'o',0x00,
    'r',0x00,
#ifdef ONBOARD_CMSIS_DAP
    // String index 7, Interface String
    20,     // Length of this string descriptor
    3,      // bDescriptorType
    'C',0x00,'M',0x00,'S',0x00,'I',0x00,'S',0x00,'-',0x00,
    'D',0x00,'A',0x00,'P',0x00,
#endif
};


typedef struct StringPointerAndLength {
    uint8_t * pointer;
    uint8_t length;
} StrPtrL;

#define STRPTRL(a) {(uint8_t *)&a[0], sizeof(a)}

StrPtrL const ReportDescriptor[] = {
    STRPTRL(report_desc_HID0),
    STRPTRL(report_desc_HID1),
    STRPTRL(report_desc_HID2),
#ifdef ONBOARD_CMSIS_DAP
    STRPTRL(report_desc_HID3),
#endif
};

uint8_t const report_desc_HID0[]=
{
    0x05, 0x01,                        // Usage Page (Generic Desktop)
    0x09, 0x06,                        // Usage (Keyboard)
    0xA1, 0x01,                        // Collection (Application)
    0x05, 0x07,                        // Usage Page (Key Codes)
    0x19, 0xE0,                        // Usage Minimum (224)
    0x29, 0xE7,                        // Usage Maximum (231)
    0x15, 0x00,                        // Logical Minimum (0)
    0x25, 0x01,                        // Logical Maximum (1)
    0x75, 0x01,                        // Report Size (1)
    0x95, 0x08,                        // Report Count (8)
    0x81, 0x02,                        // Input (Data, Variable, Absolute) -- Modifier byte

    0x95, 0x01,                        // Report Count (1)
    0x75, 0x08,                        // Report Size (8)
    0x81, 0x03,                        // (81 01) Input (Constant) -- Reserved byte

    0x95, 0x05,                        // Report Count (5)
    0x75, 0x01,                        // Report Size (1)
    0x05, 0x08,                        // Usage Page (Page# for LEDs)
    0x19, 0x01,                        // Usage Minimum (1)
    0x29, 0x05,                        // Usage Maximum (5)
    0x91, 0x02,                        // Output (Data, Variable, Absolute) -- LED report
    0x95, 0x01,                        // Report Count (1)
    0x75, 0x03,                        // Report Size (3)
    0x91, 0x03,                        // (91 03) Output (Constant) -- LED report padding

    0x95, 0x06,                        // Report Count (6)
    0x75, 0x08,                        // Report Size (8)
    0x15, 0x00,                        // Logical Minimum (0)
    0x25, 0x66,                        // Logical Maximum(102)  // was 0x65
    0x05, 0x07,                        // Usage Page (Key Codes)
    0x19, 0x00,                        // Usage Minimum (0)
    0x29, 0x66,                        // Usage Maximum (102) // was 0x65
    0x81, 0x00,                        // Input (Data, Array) -- Key arrays (6 bytes)
    0xC0                               // End Collection

};
uint8_t const report_desc_HID1[]=
{
    /* system control */
    0x05, 0x01,                      // USAGE_PAGE (Generic Desktop)
    0x09, 0x80,                      // USAGE (System Control)
    0xa1, 0x01,                      // COLLECTION (Application)
    0x85, 2,                         //   REPORT_ID (2)
    0x15, 0x01,                      //   LOGICAL_MINIMUM (0x1)
    0x26, 0xb7, 0x00,                //   LOGICAL_MAXIMUM (0xb7)
    0x19, 0x01,                      //   USAGE_MINIMUM (0x1)
    0x29, 0xb7,                      //   USAGE_MAXIMUM (0xb7)
    0x75, 0x10,                      //   REPORT_SIZE (16)
    0x95, 0x01,                      //   REPORT_COUNT (1)
    0x81, 0x00,                      //   INPUT (Data,Array,Abs)
    0xc0,                            // END_COLLECTION
    /* consumer */
    0x05, 0x0c,                      // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,                      // USAGE (Consumer Control)
    0xa1, 0x01,                      // COLLECTION (Application)
    0x85, 3,                         //   REPORT_ID (3)
    0x15, 0x01,                      //   LOGICAL_MINIMUM (0x1)
    0x26, 0x9c, 0x02,                //   LOGICAL_MAXIMUM (0x29c)
    0x19, 0x01,                      //   USAGE_MINIMUM (0x1)
    0x2a, 0x9c, 0x02,                //   USAGE_MAXIMUM (0x29c)
    0x75, 0x10,                      //   REPORT_SIZE (16)
    0x95, 0x01,                      //   REPORT_COUNT (1)
    0x81, 0x00,                      //   INPUT (Data,Array,Abs)
    0xc0,                            // END_COLLECTION
#ifdef NKRO_ENABLE
    /* nkro */
    0x05, 0x01,                      // Usage Page (Generic Desktop),
    0x09, 0x06,                      // Usage (Keyboard),
    0xA1, 0x01,                      // Collection (Application),
    0x85, 0x80,                      //   REPORT_ID (128)
    // bitmap of modifiers
    0x75, 0x01,                      //   Report Size (1),
    0x95, 0x08,                      //   Report Count (8),
    0x05, 0x07,                      //   Usage Page (Key Codes),
    0x19, 0xE0,                      //   Usage Minimum (224),
    0x29, 0xE7,                      //   Usage Maximum (231),
    0x15, 0x00,                      //   Logical Minimum (0),
    0x25, 0x01,                      //   Logical Maximum (1),
    0x81, 0x02,                      //   Input (Data, Variable, Absolute), ;Modifier byte
    // bitmap of keys
    0x95, NKRO_REPORT_KEYS * 8,      //   Report Count (),
    0x75, 0x01,                      //   Report Size (1),
    0x15, 0x00,                      //   Logical Minimum (0),
    0x25, 0x01,                      //   Logical Maximum(1),
    0x05, 0x07,                      //   Usage Page (Key Codes),
    0x19, 0x00,                      //   Usage Minimum (0),
    0x29, (NKRO_REPORT_KEYS * 8 - 1) & 0xFF,  //   Usage Maximum (),
    0x81, 0x02,                      //   Input (Data, Variable, Absolute),
    0xc0,                            // End Collection
#endif
#ifdef MOUSE_ENABLE
    /* mouse */
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x02, // USAGE (Mouse)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, 0x01, //   REPORT_ID (1)
    0x09, 0x01, //   USAGE (Pointer)
    0xa1, 0x00, //   COLLECTION (Physical)
    // ----------------------------  Buttons
    0x05, 0x09, //     USAGE_PAGE (Button)
    0x19, 0x01, //     USAGE_MINIMUM (Button 1)
    0x29, 0x05, //     USAGE_MAXIMUM (Button 5)
    0x15, 0x00, //     LOGICAL_MINIMUM (0)
    0x25, 0x01, //     LOGICAL_MAXIMUM (1)
    0x75, 0x01, //     REPORT_SIZE (1)
    0x95, 0x05, //     REPORT_COUNT (5)
    0x81, 0x02, //     INPUT (Data,Var,Abs)
    0x75, 0x03, //     REPORT_SIZE (3)
    0x95, 0x01, //     REPORT_COUNT (1)
    0x81, 0x03, //     INPUT (Cnst,Var,Abs)
    // ----------------------------  X,Y position
    0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30, //     USAGE (X)
    0x09, 0x31, //     USAGE (Y)
    0x15, 0x81, //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x02, //     REPORT_COUNT (2)
    0x81, 0x06, //     INPUT (Data,Var,Rel)
    // ----------------------------  Vertical wheel
    0x09, 0x38, //     USAGE (Wheel)
    0x15, 0x81, //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
    0x35, 0x00, //     PHYSICAL_MINIMUM (0)        - reset physical
    0x45, 0x00, //     PHYSICAL_MAXIMUM (0)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x01, //     REPORT_COUNT (1)
    0x81, 0x06, //     INPUT (Data,Var,Rel)
    // ----------------------------  Horizontal wheel
    0x05, 0x0c, //     USAGE_PAGE (Consumer Devices)
    0x0a, 0x38, 0x02, //     USAGE (AC Pan)
    0x15, 0x81, //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f, //     LOGICAL_MAXIMUM (127)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x01, //     REPORT_COUNT (1)
    0x81, 0x06, //     INPUT (Data,Var,Rel)
    0xc0, //   END_COLLECTION
    0xc0, // END_COLLECTION
#endif
};

uint8_t const report_desc_HID2[]=
{
    0x06, 0x00, 0xff,    // Usage Page (Vendor Defined)
    0x09, 0x01,    // Usage Page (Vendor Defined)
    0xa1, 0x01,    // COLLECTION (Application)
    0x85, 0x3f,    // Report ID (Vendor Defined)
    0x95, MAX_PACKET_SIZE-1,    // Report Count
    0x75, 0x08,    // Report Size
    0x25, 0x01,    // Usage Maximum
    0x15, 0x01,    // Usage Minimum
    0x09, 0x01,    // Vendor Usage
    0x81, 0x02,    // Input (Data,Var,Abs)
    0x85, 0x3f,    // Report ID (Vendor Defined)
    0x95, MAX_PACKET_SIZE-1,    //Report Count
    0x75, 0x08,    // Report Size
    0x25, 0x01,    // Usage Maximum
    0x15, 0x01,    // Usage Minimum
    0x09, 0x01,    // Vendor Usage
    0x91, 0x02,    // Ouput (Data,Var,Abs)
    0xc0    // end Application Collection
};

#ifdef ONBOARD_CMSIS_DAP
uint8_t const report_desc_HID3[] = {
    0x06, 0x00, 0xff,   // Usage Page (Vendor Defined)
    0x09, 0x01,         // Usage (0x01)
    0xa1, 0x01,         // COLLECTION (Application)
    0x15, 0x00,         // Logical Minimum (0)
    0x26, 0xFF, 0x00,   // LOGICAL_MAXIMUM(0xff)
    0x75, 0x08,         // Report Size
    0x95, MAX_PACKET_SIZE,    // Report Count
    0x09, 0x01,         // Usage (0x01)
    0x81, 0x02,         // Input (Data,Var,Abs)
    0x95, MAX_PACKET_SIZE,    // Report Count
    0x09, 0x01,         // Usage (0x01)
    0x91, 0x02,         // Ouput (Data,Var,Abs)
    0x95, MAX_PACKET_SIZE,    // Report Count
    0x09, 0x01,         // Usage (0x01)
    0xB1, 0x02,         // Feature (Data,Var,Abs)
    0xc0                // End Application Collection
};
#endif
