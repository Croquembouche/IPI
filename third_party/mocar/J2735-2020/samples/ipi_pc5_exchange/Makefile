EXE := ipi_pc5_exchange

ifeq ($(origin CXX),default)
CXX := aarch64-linux-gnu-g++
endif

LOCAL_LFLAGS ?= -L ../../lib -Wl,-rpath,'$$ORIGIN/../lib:$$ORIGIN/../../lib:/usr/lib:/usr/local/lib'
LIBS ?= -lmocarcv2x -lzlog -lpthread -ldl -lrt

IPI_ROOT := $(abspath ../../../../../cpp)
CXXFLAGS := -Wall -Wextra -Werror -O2 -std=c++17 -pthread \
            $(LOCAL_CFLAGS) -I $(IPI_ROOT)/include -I ../../include
TOOLCHAIN_ID := $(shell $(CXX) -dumpmachine 2>/dev/null || echo unknown)
OBJDIR ?= .build/$(TOOLCHAIN_ID)

OBJS := $(OBJDIR)/ipi_pc5_exchange.o \
        $(OBJDIR)/ipi_cooperative_service.o \
        $(OBJDIR)/j2735_ipi_regional_codec.o \
        $(OBJDIR)/pc5_ipi_adapter.o

all: $(EXE)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/ipi_pc5_exchange.o: ipi_pc5_exchange.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/pc5_ipi_adapter.o: $(IPI_ROOT)/src/api/pc5_ipi_adapter.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/ipi_cooperative_service.o: $(IPI_ROOT)/src/core/ipi_cooperative_service.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/j2735_ipi_regional_codec.o: $(IPI_ROOT)/src/v2x/j2735_ipi_regional_codec.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LOCAL_LFLAGS) $(LIBS)

clean:
	@rm -rf .build $(EXE)

.PHONY: all clean
