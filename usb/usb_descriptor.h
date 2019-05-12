#ifndef __USB_DESCRIPTOR__
#define __USB_DESCRIPTOR__

#define USB_VID               0x3D41    // Vendor ID (VID)
#define USB_PID               0x1919        // Product ID (PID)
#define VER_FW_H              0x08          // Device release number, in binary-coded decimal
#define VER_FW_L              0x10          // Device release number, in binary-coded decimal
#define USB_STR_INDEX_SERNUM  3
#define SIZEOF_DEVICE_DESCRIPTOR  0x12
#define EP0_PACKET_SIZE       0x08

#define MAX_PACKET_SIZE       64


uint8_t const DeviceDescriptor[SIZEOF_DEVICE_DESCRIPTOR] = {
    SIZEOF_DEVICE_DESCRIPTOR,               // Length of this descriptor
    0x01,                                   // Type code of this descriptor
    0x00, 0x02,                             // Release of USB spec
    0x00,                                   // Device's base class code
    0x00,                                   // Device's sub class code
    0x00,                                   // Device's protocol type code
    EP0_PACKET_SIZE,                        // End point 0's packet size
    USB_VID&0xFF, USB_VID>>8,               // Vendor ID for device, TI=0x0451
                                            // You can order your own VID at www.usb.org"
    USB_PID&0xFF, USB_PID>>8,               // Product ID for device,
                                            // this ID is to only with this example
    VER_FW_L, VER_FW_H,                     // Revision level of device
    1,                                      // Index of manufacturer name string desc
    2,                                      // Index of product name string desc
    USB_STR_INDEX_SERNUM,                   // Index of serial number string desc
    1                                       //  Number of configurations supported
};

#define DESCRIPTOR_TOTAL_LENGTH             sizeof(ConfigDescriptor)
#define USB_NUM_INTERFACES                  0x03
#define USB_SUPPORT_REM_WAKE                0x00    // not support
#define USB_SUPPORT_SELF_POWERED            0x80    // not self-powered
#define USB_MAX_POWER                       0xfa    // 500 mA

#define report_desc_size_HID0               sizeof(report_desc_HID0)        // 63
#define report_desc_size_HID1               sizeof(report_desc_HID1)        // 36
#define report_desc_size_HID2               sizeof(report_desc_HID2)        //
#define report_desc_size_HID3               sizeof(report_desc_HID3)        //
//#define SIZEOF_REPORT_DESCRIPTOR  36
//#define USBHID_REPORT_LENGTH      64  // length of whole HID report (including Report ID)
#define INTF_STRING_INDEX         5


#define HID0_REPORT_INTERFACE              0              // Report interface number of HID0
#define HID0_OUTEP_ADDR                    0x01           // Output Endpoint number of HID0
#define HID0_INEP_ADDR                     0x81           // Input Endpoint number of HID0

#define HID1_REPORT_INTERFACE              1              // Report interface number of HID1
#define HID1_OUTEP_ADDR                    0x02           // Output Endpoint number of HID1
#define HID1_INEP_ADDR                     0x82           // Input Endpoint number of HID1

#define HID2_REPORT_INTERFACE              2              // Report interface number of HID1
#define HID2_OUTEP_ADDR                    0x03           // Output Endpoint number of HID1
#define HID2_INEP_ADDR                     0x83           // Input Endpoint number of HID1

#define HID3_REPORT_INTERFACE              3              // Report interface number of HID1
#define HID3_OUTEP_ADDR                    0x04           // Output Endpoint number of HID1
#define HID3_INEP_ADDR                     0x84           // Input Endpoint number of HID1


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
    INTF_STRING_INDEX + 0,              // iInterface:1

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
    15,                                 // bInterval, ms

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID0_OUTEP_ADDR,                    // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    15,                                 // bInterval, ms
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
    INTF_STRING_INDEX+ 1,                 // iInterface:1

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
    10,                                  // bInterval, ms


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
    INTF_STRING_INDEX + 2,                  // iInterface:1

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
    10,                                  // bInterval, ms

    0x07,                               // bLength
    0x05,                               // bDescriptorType
    HID2_OUTEP_ADDR,                    // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    0x03,                               // bmAttributes, interrupt transfers
    0x40, 0x00,                         // wMaxPacketSize, 64 bytes
    10,                                  // bInterval, ms

    /******************************************************* end of HID**************************************/

};

/*-----------------------------------------------------------------------------+
| String Descriptor                                                            |
|-----------------------------------------------------------------------------*/
uint8_t const StringDescriptor[] = {
    // String index0, language support

    4,        // Length of language descriptor ID
    3,        // LANGID tag
    0x09, 0x04,    // 0x0409 for English


    // String index1, Manufacturer

    14,        // Length of this string descriptor
    3,        // bDescriptorType
    'L',0x00,'o',0x00,'t',0x00,'l',0x00,'a',0x00,'b',0x00,

    // String index2, Product

    30,        // Length of this string descriptor
    3,        // bDescriptorType
    'B',0x00,'L',0x00,'E',0x00,'6',0x00,'0',0x00,' ',0x00,
    'K',0x00,'e',0x00,'y',0x00,'b',0x00,'o',0x00,'a',0x00,
    'r',0x00,'d',0x00,

    // String index3, Serial Number

    14,        // Length of this string descriptor
    3,        // bDescriptorType
    '1',0x00,'1',0x00,'4',0x00,'5',0x00,'1',0x00,'4',0x00,

    // String index4, Configuration String

    22,        // Length of this string descriptor
    3,        // bDescriptorType
    'M',0x00,'S',0x00,'P',0x00,'4',0x00,'3',0x00,'0',0x00,
    ' ',0x00,'U',0x00,'S',0x00,'B',0x00,


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

        // String index6, Interface String
    32,        // Length of this string descriptor
    3,        // bDescriptorType
    'K',0x00,'e',0x00,'y',0x00,'b',0x00,'o',0x00,'a',0x00,
    'r',0x00,'d',0x00,' ',0x00,'K',0x00,'e',0x00,'y',0x00,
    'm',0x00,'a',0x00,'p',0x00,
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
};

uint8_t const report_desc_HID2[]=
{
    0x06, 0x00, 0xff,    // Usage Page (Vendor Defined)
    0x09, 0x01,    // Usage Page (Vendor Defined)
    0xa1, 0x01,    // COLLECTION (Application)
    0x85, 0x3f,    // Report ID (Vendor Defined)
    0x95, 0x04,    // Report Count
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
    0x91 ,0x02,    // Ouput (Data,Var,Abs)
    0xc0    // end Application Collection
};



#endif // __USB_DESCRIPTOR__
