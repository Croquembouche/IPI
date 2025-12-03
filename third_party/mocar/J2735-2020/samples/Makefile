EXE:=samples

ifeq ($(platform),imx6)
	TOOLCHAIN := /opt/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf
	TARGET_CC=$(TOOLCHAIN)/bin/arm-linux-gnueabihf-gcc
	TARGET_LD=$(TOOLCHAIN)/bin/arm-linux-gnueabihf-ld
	TARGET_AR=$(TOOLCHAIN)/bin/arm-linux-gnueabihf-ar
else
	TOOLCHAIN := /opt/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu
	TARGET_CC=$(TOOLCHAIN)/bin/aarch64-linux-gnu-gcc
	TARGET_LD=$(TOOLCHAIN)/bin/aarch64-linux-gnu-ld
	TARGET_AR=$(TOOLCHAIN)/bin/aarch64-linux-gnu-ar
endif



CC_FLAGS:=-Wall -Werror -fstack-protector -fstack-protector-all

export CR_CC:=$(TARGET_CC) $(CC_FLAGS)
export CR_AR:=$(TARGET_AR)


top_dir:=$(shell pwd)/..

sub_dirs:= $(wildcard */)

$(info $(sub_dirs))

export LOCAL_CFLAGS:= -I $(top_dir)/location/include \
					  -I $(top_dir)/include
export LOCAL_LFLAGS:= -L $(top_dir)/lib/$(platform)
export LIBS:= -lmocarcv2x -lpthread -ldl  -lrt -lzlog

$(EXE): clean 
	@ for dir in $(sub_dirs); do make TPDIR=$(TOP_DIR)/$(EXE) -C $$dir; if [ "$$?" -ne "0" ]; then exit 3; fi; done

clean:
	@ for dir in $(sub_dirs); do make TPDIR=$(TOP_DIR)/$(EXE) -C $$dir clean;  if [ "$$?" -ne "0" ]; then exit 3; fi; done

