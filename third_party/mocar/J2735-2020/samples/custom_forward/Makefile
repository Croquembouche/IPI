EXE:=custom_forward

# Force aarch64 cross-compiler for C++ build
CXX := /opt/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++

SRCS:= $(wildcard *.cpp)
objects += $(patsubst %.cpp,%.o,$(SRCS))

%.o: %.cpp
	$(CXX) -std=c++14 -Wall -Werror -fstack-protector -fstack-protector-all -c -g $< -o $@ $(LOCAL_CFLAGS)

$(EXE): clean  $(objects)
	$(CXX) -std=c++14 -o $@ $(objects) $(LOCAL_LFLAGS) $(LIBS)

clean:
	@ rm -rf *.o $(EXE)
