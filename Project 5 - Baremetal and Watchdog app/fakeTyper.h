// fakeTyper.h: Module to simulate typing on the screen.
#ifndef _FAKETYPER_H_
#define _FAKETYPER_H_

void FakeTyper_init(void);
void FakeTyper_setMessage(char *newMessage);
void FakeTyper_notifyOnTimeIsr(void);
void FakeTyper_doBackgroundWork(void);


#endif
