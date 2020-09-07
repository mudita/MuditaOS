# c-template project (https://gitlab.com/arturmadrzak/c-template)
# Copyright (c) 2020 Artur Mądrzak <artur@madrzak.eu>

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

CFLAGS = -Wall -fPIC -MMD -DNDEBUG

.PHONY: all lib test clean

all: lib test

lib: libmtp.a

test: lib
	make -C tests

libmtp.a: $(OBJS)
	$(AR) csr $@ $^

clean:
	make -C tests clean
	rm -rf $(TESTDIR)
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f libmtp.a

include $(wildcard *.d)
