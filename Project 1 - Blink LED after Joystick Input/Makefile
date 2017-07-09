#makefile for assignment 1 hello embedded world
#cross compile the c file for the ARM architecture
#outputs the compile file to the public/myApps folder

CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

app:
	$(CC_C) $(CFLAGS) hello-world.c -o hello-world
	cp hello-world ~/cmpt433/public/myApps/

clean:
	rm hello-world

