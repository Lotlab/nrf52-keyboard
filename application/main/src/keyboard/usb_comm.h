#if !defined(__USB_COMM_H__)
#define __USB_COMM_H__

#include <stdbool.h>
#include <stdint.h>

bool usb_working(void);
void usb_send(uint8_t index, uint8_t len, uint8_t* pattern);
void usb_comm_init(void);
void usb_comm_sleep_prepare(void);

#endif // __USB_COMM_H__
