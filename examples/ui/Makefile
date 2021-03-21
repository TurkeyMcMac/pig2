exe = ui
CFLAGS = -I ../.. -std=c99 -Wall -Wextra -pedantic
LDLIBS = -lcurses

CC ?= cc
RM ?= rm -f

$(exe): src/* ../../pig2.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) src/*.c $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(exe)
