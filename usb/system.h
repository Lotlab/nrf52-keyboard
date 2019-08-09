#pragma once

#include <stdint.h>
#define FREQ_SYS 24000000

void InitUART();
void CfgSysClock();
void DelayMs(uint16_t n);
void PrintHex(uint8_t *data, uint8_t len);
int putchar(int c);
