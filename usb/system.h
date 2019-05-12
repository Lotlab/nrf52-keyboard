#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define FREQ_SYS 12000000

void InitUART();
void CfgSysClock();
void DelayMs(uint16_t n);
void PrintHex(uint8_t *data, uint8_t len);

#endif // __SYSTEM_H__
