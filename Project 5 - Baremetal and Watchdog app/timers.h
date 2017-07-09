// timers.h: Timer and the watchdog

#ifndef _TIMERS_H_
#define _TIMERS_H_
#include <stdbool.h>

// Timer tick controls:
void Timers_timerInit(void);
_Bool Timers_isIsrFlagSet(void);
void Timers_clearIsrFlag(void);

// Watchdog timer controls:
void Timers_watchdogInit(void);
void Timers_hitWatchdog(void);

#endif
