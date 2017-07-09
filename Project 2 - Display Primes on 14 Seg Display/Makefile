#makefile for assignment 1 hello embedded world
#cross compile the c file for the ARM architecture
#outputs the compile file to the public/myApps folder

CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -pthread -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

app:
	$(CC_C) $(CFLAGS) main.c  primeFinder.c udpListener.c A2DPot.c 14SegDisplay.c -o primer -lm
	cp primer ~/cmpt433/public/myApps/

	$(CC_C) $(CFLAGS) noworky.c -o works 
	cp works ~/cmpt433/public/myApps/
clean:
	rm primeAS2
	rm works


