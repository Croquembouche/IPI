EXE := ipi_spat_bridge

# Use the same toolchain naming as other samples; C++17 for bridge + IPI helpers.
CXX := aarch64-linux-gnu-g++

# Defaults if not provided by parent samples Makefile
LOCAL_LFLAGS ?= -L ../../lib -Wl,-rpath,'$$ORIGIN/../../lib:/usr/lib:/usr/local/lib'
LIBS ?= -lmocarcv2x -lzlog -ldl -lrt

IPI_ROOT := $(abspath ../../../../../cpp)
IPI_INC := -I $(IPI_ROOT)/include
CXXFLAGS := -Wall -O2 -std=c++17 -pthread $(LOCAL_CFLAGS) $(IPI_INC) -I ../../include
TOOLCHAIN_ID := $(shell $(CXX) -dumpmachine 2>/dev/null || echo unknown)
OBJDIR ?= .build/$(TOOLCHAIN_ID)

SRCS := ipi_spat_bridge.cpp \
        $(IPI_ROOT)/src/v2x/uper_codec.cpp \
        $(IPI_ROOT)/src/v2x/j2735_messages.cpp

OBJS := $(OBJDIR)/ipi_spat_bridge.o \
        $(OBJDIR)/uper_codec.o \
        $(OBJDIR)/j2735_messages.o

all: $(EXE)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/ipi_spat_bridge.o: ipi_spat_bridge.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/uper_codec.o: $(IPI_ROOT)/src/v2x/uper_codec.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/j2735_messages.o: $(IPI_ROOT)/src/v2x/j2735_messages.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LOCAL_LFLAGS) $(LIBS)

clean:
	@rm -rf .build $(EXE)
