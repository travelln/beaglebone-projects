
#
# Locating the root directory
#
# (Unable to use ~; spell out full path or use HOME)
ROOT=${HOME}/cmpt433/AM335X_StarterWare_02_00_01_01/

# Path to compiler
LIB_PATH=${HOME}/cmpt433/linaro-gcc/gcc-arm-none-eabi-4_7-2012q4
export LIB_PATH

#
# Device and EVM definitions
#
DEVICE=am335x
EVM=beaglebone

#
# Include the makefile definitions. This contains compiler, linker and
# archiver definitions and options
#
include ${ROOT}/build/armv7a/gcc/makedefs

#
# Target Directories that need to be built
#
DIRS=${DRIVERS_BLD} ${PLATFORM_BLD} ${SYSCONFIG_BLD} ${UTILITY_BLD}

#
# The application name
# 
APPNAME=lightBouncer

#
# Where the application will be loaded to. This is required to generate
# image with Header (Load Address and Size)
#
IMG_LOAD_ADDR = 0x80000000

#
# Application Location
#
APP=./
DEPLOY=~/cmpt433/public/baremetal/

#
# Application source files
#
APP_SRC=$(APP)/*.c

#
# Required library files
#
APP_LIB=-ldrivers  \
	-lutils    \
	-lplatform \
	-lsystem_config

CFLAGS+=-std=c99 -Wall -Werror
#
# Rules for building the application and library
#
# Clean first because otherwise every other build fails. 
# (some problem with files in the build directory).
#all: debug release
all: clean debug
#all: release

debug:
	make TARGET_MODE=debug lib
	make TARGET_MODE=Debug bin

release:
	make TARGET_MODE=release lib
	make TARGET_MODE=Release bin

lib:
	@for i in ${DIRS};				\
	do						\
		if [ -f $${i}/makefile ] ;		    \
		then					  \
			make $(TARGET_MODE) -C $${i} || exit $$?; \
		fi;					   \
	done;


bin:
	$(CC)  $(CFLAGS) $(APP_SRC)
	@mkdir -p $(TARGET_MODE)/
	@mv *.o* $(TARGET_MODE)/
	$(LD) ${LDFLAGS} ${LPATH} -o $(TARGET_MODE)/$(APPNAME).out \
          -Map $(TARGET_MODE)/$(APPNAME).map $(TARGET_MODE)/*.o* \
          $(APP_LIB) -lc -lgcc $(APP_LIB) $(RUNTIMELIB) -T load_script.lds
	$(BIN) $(BINFLAGS) $(TARGET_MODE)/$(APPNAME).out \
               $(TARGET_MODE)/$(APPNAME).bin 
	cd $(ROOT)/tools/ti_image/; gcc tiimage.c -o a.out; cd - 
	       $(ROOT)/tools/ti_image/a.out $(IMG_LOAD_ADDR) NONE \
               $(TARGET_MODE)/$(APPNAME).bin \
               $(TARGET_MODE)/$(APPNAME)_ti.bin; rm -rf $(ROOT)/tools/ti_image/a.out;
	cp $(TARGET_MODE)/$(APPNAME).bin $(DEPLOY)/$(APPNAME).bin

#
# Rules for cleaning
#
clean:
	@rm -rf Debug Release $(DEPLOY)/$(APPNAME).bin

clean+: clean
	@make TARGET_MODE=clean lib


