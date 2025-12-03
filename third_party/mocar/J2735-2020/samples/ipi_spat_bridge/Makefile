EXE := ipi_spat_bridge

# Use the same toolchain naming as other samples; C++17 for bridge + IPI helpers.
CXX := aarch64-linux-gnu-g++

# Defaults if not provided by parent samples Makefile
LOCAL_LFLAGS ?= -L ../../lib -Wl,-rpath,'$$ORIGIN/../../lib:/usr/lib:/usr/local/lib'
LIBS ?= -lmocarcv2x -lzlog -ldl -lrt

IPI_ROOT := $(abspath ../../../../../cpp)
IPI_INC := -I $(IPI_ROOT)/include
CXXFLAGS := -Wall -O2 -std=c++17 -pthread $(LOCAL_CFLAGS) $(IPI_INC) -I ../../include

SRCS := ipi_spat_bridge.cpp \
        $(IPI_ROOT)/src/v2x/uper_codec.cpp \
        $(IPI_ROOT)/src/v2x/j2735_messages.cpp

OBJS := $(SRCS:.cpp=.o)

all: $(EXE)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LOCAL_LFLAGS) $(LIBS)

clean:
	@rm -f $(OBJS) $(EXE)
