EXE:=nmea_sample

SRCS:= $(wildcard *.c)

$(info $(SRCS))
objects += $(patsubst %.c,%.o,$(SRCS))

%.o: %.c
	$(CR_CC) -c -g $< -o $@ $(LOCAL_CFLAGS)

$(EXE): clean  $(objects)
	$(CR_CC) -o $@ $(objects) $(LOCAL_LFLAGS) $(LIBS)

clean:
	@ rm -rf *.o $(EXE)
