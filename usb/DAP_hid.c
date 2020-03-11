#include "CH554_SDCC.h"
#include "DAP.h"
#include "endpoints.h"
#include <string.h>

#define EP4_OUT_BUF (&Ep0Buffer[64])
#define EP4_IN_BUF (&EP4_OUT_BUF[64])

volatile static uint8_t ep4_len = 0;

void EP4_OUT()
{
    if (ep4_len > 0) {
        EP_OUT_RESP(4, UEP_T_RES_NAK);
        return;
    }

    if (USB_RX_LEN == 0) {
        return;
    }

    if (EP4_OUT_BUF[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1;
        return;
    }

    EP_OUT_RESP(4, UEP_T_RES_ACK);
    ep4_len = USB_RX_LEN;
}

void Dap_Init()
{
    DAP_Setup();
}

void Dap_Routine()
{
    if (ep4_len > 0 && !usb_state.is_busy) {
        uint8_t len = DAP_ExecuteCommand(EP4_OUT_BUF, EP4_IN_BUF) & 0xFF;
        if (len <= MAX_PACKET_SIZE) {
            usb_state.is_busy = true;
            UEP4_T_LEN = MAX_PACKET_SIZE;
            EP_IN_RESP(4, UEP_T_RES_ACK);
        }

        ep4_len = 0;
    }
}
