#if !defined(__SLEEP_REASON__H)
#define __SLEEP_REASON__H

#include <stdbool.h>

bool sleep_reason_get(void);
void sleep_reason_set(bool val);

#endif // __SLEEP_REASON__H
