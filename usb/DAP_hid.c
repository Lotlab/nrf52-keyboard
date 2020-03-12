#include "CH554_SDCC.h"
#include "DAP.h"
#include "endpoints.h"
#include <string.h>

#define EP4_OUT_BUF (&Ep0Buffer[64])
#define EP4_IN_BUF (&Ep0Buffer[128])

volatile static uint8_t ep4_len = 0;

void EP4_OUT()
{
    if (ep4_len > 0) {
        UEP4_CTRL = UEP4_CTRL ^ bUEP_R_TOG & ~(MASK_UEP_R_RES) | UEP_R_RES_NAK;
        return;
    }

    if (USB_RX_LEN == 0) {
        return;
    }

    if (EP4_OUT_BUF[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1;
        return;
    }

    ep4_len = USB_RX_LEN;
    UEP4_CTRL = UEP4_CTRL ^ bUEP_R_TOG & ~(MASK_UEP_R_RES) | UEP_R_RES_ACK;
}

void Dap_Init()
{
    DAP_Setup();
}

void Dap_Routine()
{
    if (ep4_len > 0 && !usb_state.is_busy) {
        for (uint8_t i = 0; i < MAX_PACKET_SIZE; i++)
            Ep0Buffer[i + 128] = 0;
        uint8_t len = DAP_ExecuteCommand(EP4_OUT_BUF, EP4_IN_BUF) & 0xFF;
        usb_state.is_busy = true;
        UEP4_T_LEN = MAX_PACKET_SIZE;
        UEP4_CTRL = UEP4_CTRL & ~(MASK_UEP_T_RES) | UEP_T_RES_ACK;

        ep4_len = 0;
    }
}
