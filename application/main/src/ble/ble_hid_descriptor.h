#pragma once

#include "report.h"

static uint8_t hid_descriptor[] = {
    0x05, 0x01, // Usage Page (Generic Desktop)
    0x09, 0x06, // Usage (Keyboard)
    0xA1, 0x01, // Collection (Application)
    0x85, 0x7f, // Report ID 0x7F
    0x05, 0x07, // Usage Page (Key Codes)
    0x19, 0xe0, // Usage Minimum (224)
    0x29, 0xe7, // Usage Maximum (231)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x01, // Logical Maximum (1)
    0x75, 0x01, // Report Size (1)
    0x95, 0x08, // Report Count (8)
    0x81, 0x02, // Input (Data, Variable, Absolute)

    0x95, 0x01, // Report Count (1)
    0x75, 0x08, // Report Size (8)
    0x81, 0x01, // Input (Constant) reserved byte(1)

    0x95, 0x05, // Report Count (5)
    0x75, 0x01, // Report Size (1)
    0x05, 0x08, // Usage Page (Page# for LEDs)
    0x19, 0x01, // Usage Minimum (1)
    0x29, 0x05, // Usage Maximum (5)
    0x91, 0x02, // Output (Data, Variable, Absolute), Led report
    0x95, 0x01, // Report Count (1)
    0x75, 0x03, // Report Size (3)
    0x91, 0x01, // Output (Data, Variable, Absolute), Led report padding

    0x95, 0x06, // Report Count (6)
    0x75, 0x08, // Report Size (8)
    0x15, 0x00, // Logical Minimum (0)
    0x25, 0x65, // Logical Maximum (101)
    0x05, 0x07, // Usage Page (Key codes)
    0x19, 0x00, // Usage Minimum (0)
    0x29, 0x65, // Usage Maximum (101)
    0x81, 0x00, // Input (Data, Array) Key array(6 bytes)

    0xC0, // End Collection (Application)

#ifdef MOUSEKEY_ENABLE
    /* mouse */
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x02, // USAGE (Mouse)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, REPORT_ID_MOUSE, //   REPORT_ID (1)
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

#ifdef EXTRAKEY_ENABLE
    // system
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x80, // USAGE (System Control)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, REPORT_ID_SYSTEM, //   REPORT_ID (2)
    0x15, 0x01, //   LOGICAL_MINIMUM (0x1)
    0x26, 0xb7, 0x00, //   LOGICAL_MAXIMUM (0xb7)
    0x19, 0x01, //   USAGE_MINIMUM (0x1)
    0x29, 0xb7, //   USAGE_MAXIMUM (0xb7)
    0x75, 0x10, //   REPORT_SIZE (16)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x81, 0x00, //   INPUT (Data,Array,Abs)
    0xc0, // END_COLLECTION

    // consumer
    0x05, 0x0c, // USAGE_PAGE (Consumer Devices)
    0x09, 0x01, // USAGE (Consumer Control)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, REPORT_ID_CONSUMER, //   REPORT_ID (3)
    0x15, 0x01, //   LOGICAL_MINIMUM (0x1)
    0x26, 0x9c, 0x02, //   LOGICAL_MAXIMUM (0x29c)
    0x19, 0x01, //   USAGE_MINIMUM (0x1)
    0x2a, 0x9c, 0x02, //   USAGE_MAXIMUM (0x29c)
    0x75, 0x10, //   REPORT_SIZE (16)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x81, 0x00, //   INPUT (Data,Array,Abs)
    0xc0, // END_COLLECTION
#endif
};