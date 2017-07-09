// timers.c: Handle 10ms and Watchdog timers.

#include "timers.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "error.h"
#include "interrupt.h"
#include "dmtimer.h"
#include "watchdog.h"

#include "serial.h"
#include "fakeTyper.h"

/******************************************************************************
 **                     TIMER TICK FUNCTIONS
 *******************************************************************************/
#define TIMER_INITIAL_COUNT             (0xFFE00000)  // Some time that looks good.
#define TIMER_RLD_COUNT                 TIMER_INITIAL_COUNT

static void DMTimerAintcConfigure(void);
static void DMTimerSetUp(void);
static void DMTimerIsr(void);

static volatile _Bool flagIsr = 0;

void Timers_timerInit(void)
{
	/* This function will enable clocks for the DMTimer2 instance */
	DMTimer2ModuleClkConfig();

	/* Enable IRQ in CPSR */
	IntMasterIRQEnable();

	/* Register DMTimer2 interrupts on to AINTC */
	DMTimerAintcConfigure();

	/* Perform the necessary configurations for DMTimer */
	DMTimerSetUp();

	// Extra setup:
	// ..Select clock
	// ..Set prescaler
	DMTimerPreScalerClkDisable(SOC_DMTIMER_2_REGS);

	/* Enable the DMTimer interrupts */
	DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

	/* Start the DMTimer */
	DMTimerEnable(SOC_DMTIMER_2_REGS);
}

_Bool Timers_isIsrFlagSet(void)
{
	return flagIsr;
}
void Timers_clearIsrFlag(void)
{
	flagIsr = false;
}


// Timer ISR
static void DMTimerIsr(void)
{
	/* Disable the DMTimer interrupts */
	DMTimerIntDisable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

	/* Clear the status of the interrupt flags */
	DMTimerIntStatusClear(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_IT_FLAG);

	// --------------------------------------------------------
	// Work done in the ISR:
	// .. Record ISR state for background task to poll.
	flagIsr = true;
	// .. Update fake typing
	FakeTyper_notifyOnTimeIsr();
	// --------------------------------------------------------

	/* Enable the DMTimer interrupts */
	DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);
}




static void DMTimerAintcConfigure(void)
{
	/* Initialize the ARM interrupt control */
	// NOTE: Must disable this, otherwise clears any already established
	// interrupts, such as the serial Rx.
	//IntAINTCInit();

	/* Registering DMTimerIsr */
	IntRegister(SYS_INT_TINT2, DMTimerIsr);

	/* Set the priority */
	IntPrioritySet(SYS_INT_TINT2, 0, AINTC_HOSTINT_ROUTE_IRQ);

	/* Enable the system interrupt */
	IntSystemEnable(SYS_INT_TINT2);
}

static void DMTimerSetUp(void)
{
	/* Load the counter with the initial count value */
	DMTimerCounterSet(SOC_DMTIMER_2_REGS, TIMER_INITIAL_COUNT);

	/* Load the load register with the reload count value */
	DMTimerReloadSet(SOC_DMTIMER_2_REGS, TIMER_RLD_COUNT);

	/* Configure the DMTimer for Auto-reload and compare mode */
	DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);
}

/******************************************************************************
 **                      WATCHDOG FUNCTIONS
 *******************************************************************************/
#define WD_CLOCK          (32000L)
#define WD_TIMEOUT_S      (30)
#define WD_TIMEOUT_TICKS  (WD_TIMEOUT_S * WD_CLOCK)
#define WD_RESET_VALUE    ((unsigned int)0xFFFFFFFF - WD_TIMEOUT_TICKS + 1)

void Timers_watchdogInit()
{
	WatchdogTimer1ModuleClkConfig();
	WatchdogTimerReset(SOC_WDT_1_REGS);
	WatchdogTimerDisable(SOC_WDT_1_REGS);
	WatchdogTimerPreScalerClkDisable(SOC_WDT_1_REGS);
	WatchdogTimerReloadSet(SOC_WDT_1_REGS, WD_RESET_VALUE);
	WatchdogTimerEnable(SOC_WDT_1_REGS);
}

void Timers_hitWatchdog(void)
{
	static unsigned int triggerCounter = 0;

	// Hit the WD, giving it a new trigger each time to keep it from
	// resetting the board.
	triggerCounter++;

	WatchdogTimerTriggerSet(SOC_WDT_1_REGS, triggerCounter);
}



