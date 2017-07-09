CC = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -std=c99 -g -D_POSIX_C_SOURCE=200809L
OUTFILE = beatbox
OUTDIR = $(HOME)/cmpt433/public/myApps
SOURCE = $(wildcard *.c)
HEADER = $(wildcard *.h)
OBJ = $(SOURCE) $(HEADER)
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB

all: $(OBJ) wav node
	$(CC) $(CFLAGS) $(SOURCE) -o $(OUTDIR)/$(OUTFILE) $(LFLAGS) -lpthread -lasound

wav:
	mkdir -p $(OUTDIR)/beatbox-wav-files/ 
	cp -R beatbox-wave-files/* $(OUTDIR)/beatbox-wav-files/ 
node:
	mkdir -p $(OUTDIR)/beatbox-server-copy/ 
	cp -R as3-server/* $(OUTDIR)/beatbox-server-copy/ 