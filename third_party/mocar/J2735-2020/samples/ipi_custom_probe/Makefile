EXE := ipi_custom_probe

ifeq ($(origin CC),default)
CC := aarch64-linux-gnu-gcc
endif

COMMON_CFLAGS ?= -Wall -Werror -fstack-protector -fstack-protector-all
CR_CC ?= $(CC) $(COMMON_CFLAGS)
LOCAL_CFLAGS ?= -I ../../include
LOCAL_LFLAGS ?= -L ../../lib -Wl,-rpath,'$$ORIGIN/../lib:$$ORIGIN/../../lib:/usr/lib:/usr/local/lib'
LIBS ?= -lmocarcv2x -ldl -lrt -lzlog
CSTD_FLAGS := -std=c11 -D_POSIX_C_SOURCE=200809L

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(EXE)

%.o: %.c
	$(CR_CC) $(CSTD_FLAGS) -c -g $< -o $@ $(LOCAL_CFLAGS)

$(EXE): clean $(OBJS)
	$(CR_CC) $(CSTD_FLAGS) -o $@ $(OBJS) $(LOCAL_LFLAGS) $(LIBS)

clean:
	@rm -rf *.o $(EXE)

.PHONY: all clean
