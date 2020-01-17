#pragma once
#include "report.h"
#include <stdint.h>

extern uint8_t passkey[];
void passkey_req_handler(void);
void passkey_input_handler(uint8_t len, uint8_t* keys);
