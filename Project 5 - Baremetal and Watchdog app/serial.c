//Purpose: intitialize the UART and read values from the user.
//  Displays the help message and triggers events based on user input.

#include "uart_irda_cir.h"
#include "soc_AM335x.h"
#include "interrupt.h"
#include "beaglebone.h"
#include "consoleUtils.h"
#include "hw_types.h"
#include <stdint.h>
#include "watchdog.h"
#include "dmtimer.h"
#include "serial.h"
#include "fakeTyper.h"
#include "leds.h"
#include "joystick.h"

/******************************************************************************
 **              INTERNAL MACRO DEFINITIONS
 ******************************************************************************/
#define BAUD_RATE_115200          (115200)
#define UART_MODULE_INPUT_CLK     (48000000)

/******************************************************************************
 **              INTERNAL FUNCTION PROTOTYPES
 ******************************************************************************/
static void UARTIsr(void);
static void UartInterruptEnable(void);
static void UartBaudRateSet(void);


static volatile uint8_t rxByte = 0;




/******************************************************************************
 **              FUNCTION DEFINITIONS
 ******************************************************************************/

void Serial_init(void)
{
	/* Configuring the system clocks for UART0 instance. */
	UART0ModuleClkConfig();
	/* Performing the Pin Multiplexing for UART0 instance. */
	UARTPinMuxSetup(0);
	/* Performing a module reset. */
	UARTModuleReset(SOC_UART_0_REGS);
	/* Performing Baud Rate settings. */
	UartBaudRateSet();
	/* Switching to Configuration Mode B. */
	UARTRegConfigModeEnable(SOC_UART_0_REGS, UART_REG_CONFIG_MODE_B);
	/* Programming the Line Characteristics. */
	UARTLineCharacConfig(SOC_UART_0_REGS,
			(UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1),
			UART_PARITY_NONE);
	/* Disabling write access to Divisor Latches. */
	UARTDivisorLatchDisable(SOC_UART_0_REGS);
	/* Disabling Break Control. */
	UARTBreakCtl(SOC_UART_0_REGS, UART_BREAK_COND_DISABLE);
	/* Switching to UART16x operating mode. */
	UARTOperatingModeSelect(SOC_UART_0_REGS, UART16x_OPER_MODE);
	/* Select the console type based on compile time check */
	ConsoleUtilsSetType(CONSOLE_UART);
	/* Performing Interrupt configurations. */
	UartInterruptEnable();
}


void Serial_doBackgroundWork(void)
{
	if (rxByte != 0) {
		// Tell a joke
		
		if (rxByte == '?') {


			ConsoleUtilsPrintf("\nCommands:\n");	
			ConsoleUtilsPrintf("?    : Display this help message.\n");
			ConsoleUtilsPrintf("0-9  : Set speed 0 (slow) to 9 (fast)\n");
			ConsoleUtilsPrintf("a    : Select pattern A (bounce).\n");
			ConsoleUtilsPrintf("b    : Select patter B (bar).\n");
			ConsoleUtilsPrintf("x    : Stop hitting the watchdog.\n");

			//FakeTyper_setMessage(jokes[curJoke]);
			//curJoke = (curJoke + 1) % NUM_JOKES;
		}
		
		else if (rxByte == 'x') {
		
			DMTimerDisable(SOC_DMTIMER_2_REGS);
		}
		else if (rxByte == 'a') {
		
			initializeLeds();

			driveLedsWithSWFunction();
		
			//ConsoleUtilsPrintf("\nJoystick is pressed up:\n");
		
		}
		
		else if (rxByte == 'b') {
		
		initializeLeds();
		
		driveLedsWithSetAndClear();

		}
		
		else {
			ConsoleUtilsPrintf("\nUnknown Command - Showing Help:\n");
			ConsoleUtilsPrintf("\nCommands:\n");	
			ConsoleUtilsPrintf("?    : Display this help message.\n");
			ConsoleUtilsPrintf("0-9  : Set speed 0 (slow) to 9 (fast)\n");
			ConsoleUtilsPrintf("a    : Select pattern A (bounce).\n");
			ConsoleUtilsPrintf("b    : Select patter B (bar).\n");
			ConsoleUtilsPrintf("x    : Stop hitting the watchdog.\n");
		
		}

		rxByte = 0;
	}


}


/******************************************************************************
 **              INTERNAL FUNCTIONS
 ******************************************************************************/

/*
 ** A wrapper function performing Baud Rate settings.
 */
static void UartBaudRateSet(void)
{
	unsigned int divisorValue = 0;

	/* Computing the Divisor Value. */
	divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
			BAUD_RATE_115200,
			UART16x_OPER_MODE,
			UART_MIR_OVERSAMPLING_RATE_42);

	/* Programming the Divisor Latches. */
	UARTDivisorLatchWrite(SOC_UART_0_REGS, divisorValue);
}

/*
 ** A wrapper function performing Interrupt configurations.
 */
static void UartInterruptEnable(void)
{
	/* Enabling IRQ in CPSR of ARM processor. */
	IntMasterIRQEnable();

	/* Configuring AINTC to receive UART0 interrupts. */
	/* ..Initializing the ARM Interrupt Controller. */
	IntAINTCInit();
	/* ..Registering the Interrupt Service Routine(ISR). */
	IntRegister(SYS_INT_UART0INT, UARTIsr);
	/* ..Setting the priority for the system interrupt in AINTC. */
	IntPrioritySet(SYS_INT_UART0INT, 0, AINTC_HOSTINT_ROUTE_IRQ);
	/* ..Enabling the system interrupt in AINTC. */
	IntSystemEnable(SYS_INT_UART0INT);

	/* Enabling the specified UART interrupts. */
	UARTIntEnable(SOC_UART_0_REGS, (UART_INT_RHR_CTI));
}

/*
 ** Interrupt Service Routine for UART.
 */
static void UARTIsr(void)
{
	unsigned int intId = 0;

	/* Checking the source of UART interrupt. */
	intId = UARTIntIdentityGet(SOC_UART_0_REGS);

	switch(intId) {

	case UART_INTID_RX_THRES_REACH:
		rxByte = UARTCharGetNonBlocking(SOC_UART_0_REGS);
		UARTCharPutNonBlocking(SOC_UART_0_REGS, rxByte);
		break;

	default:
		break;
	}

}
