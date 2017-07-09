// fakeTyper.c: Module to simulate typing on the screen.
#include "fakeTyper.h"
#include "consoleUtils.h"
#include <stdbool.h>

static volatile _Bool isTimeToPrintChar = false;

char *message = 0;

void FakeTyper_init(void)
{
	// Often would have something to do here in other modules,
	// so included here as just a sample.
}

void FakeTyper_setMessage(char *newMessage)
{
	message = newMessage;
}

void FakeTyper_notifyOnTimeIsr(void)
{
	isTimeToPrintChar = true;
}

void FakeTyper_doBackgroundWork(void)
{
	if (isTimeToPrintChar) {
		isTimeToPrintChar = false;

		// Do we have a message, and not at its end?
		if (message && *message) {
			ConsoleUtilsPutChar(*message);
			message++;
		}
	}
}
