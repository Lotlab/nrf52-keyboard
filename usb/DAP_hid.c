#include "DAP_hid.h"
#include "CH554_SDCC.h"
#include "DAP_queue.h"
#include "endpoints.h"
#include <string.h>

#define EP4_OUT_BUF (&Ep0Buffer[64])
#define EP4_IN_BUF (&EP4_OUT_BUF[64])

static __XDATA DAP_queue DAP_Cmd_queue;

void dap_init()
{
    DAP_queue_init(&DAP_Cmd_queue);
}

bool dap_EP_IN()
{
    uint8_t* sbuf;
    int slen;
    if (DAP_queue_get_send_buf(&DAP_Cmd_queue, &sbuf, &slen)) {
        if (slen <= MAX_PACKET_SIZE) {
            memcpy(EP4_IN_BUF, sbuf, slen);
            UEP4_T_LEN = MAX_PACKET_SIZE;
        }
        return true;
    } else {
        usb_state.is_busy = false;
        return false;
    }
}

void EP4_OUT()
{
    uint8_t* rbuf;
    if (USB_RX_LEN == 0) {
        return;
    }

    if (EP4_OUT_BUF[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1;
        return;
    }

    // execute and store to DAP_queue
    if (DAP_queue_execute_buf(&DAP_Cmd_queue, EP4_OUT_BUF, USB_RX_LEN, &rbuf)) {
        if (!usb_state.is_busy) {
            usb_state.is_busy = true;
        }
    }
}
