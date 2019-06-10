#ifndef __ENDPOINTS_H__
#define __ENDPOINTS_H__
#include <stdint.h>
#include <stdbool.h>

extern uint8_t __xdata __at(0x00) Ep0Buffer[];
extern uint8_t __xdata __at(0x0a) Ep1Buffer[];
extern uint8_t __xdata __at(0x90) Ep2Buffer[];
extern uint8_t __xdata __at(0xB0) Ep3Buffer[];

extern bool usb_ready;

extern void USBDeviceInit();

extern void EP0_OUT();
extern void EP0_IN();
extern void EP0_SETUP();

extern void EP1_IN();
extern void EP1_OUT();

extern void EP2_IN();
extern void EP2_OUT();

extern void EP3_IN();
extern void EP3_OUT();

extern void EP4_IN();
extern void EP4_OUT();

extern void nop();

#define USB_OUT_EP0 EP0_OUT
#define USB_OUT_EP1 EP1_OUT
#define USB_OUT_EP2 nop
#define USB_OUT_EP3 EP3_OUT
#define USB_OUT_EP4 nop

#define USB_IN_EP0 EP0_IN
#define USB_IN_EP1 EP1_IN
#define USB_IN_EP2 EP2_IN
#define USB_IN_EP3 EP3_IN
#define USB_IN_EP4 nop

#define USB_SETUP_EP0 EP0_SETUP
#define USB_SETUP_EP1 nop
#define USB_SETUP_EP2 nop
#define USB_SETUP_EP3 nop
#define USB_SETUP_EP4 nop

#define USB_SOF_EP0 nop
#define USB_SOF_EP1 nop
#define USB_SOF_EP2 nop
#define USB_SOF_EP3 nop
#define USB_SOF_EP4 nop

#endif // __ENDPOINTS_H__
