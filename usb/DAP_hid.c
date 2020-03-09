#include "CH554_SDCC.h"
#include "DAP.h"
#include "endpoints.h"
#include <string.h>

#define EP4_OUT_BUF (&Ep0Buffer[64])
#define EP4_IN_BUF (&EP4_OUT_BUF[64])

void EP4_OUT()
{
    if (USB_RX_LEN == 0) {
        return;
    }

    if (EP4_OUT_BUF[0] == ID_DAP_TransferAbort) {
        DAP_TransferAbort = 1;
        return;
    }

    // execute and store to DAP_queue
    UEP4_T_LEN = DAP_ExecuteCommand(EP4_OUT_BUF, EP4_IN_BUF) & 0xFF;
}
