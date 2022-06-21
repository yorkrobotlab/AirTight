CC ?= clang
PREFIX ?= /usr/local
XBEE_DEFINES ?= \
	-DXBEE_PLATFORM_HEADER='"ports/posix/platform_config.h"' \
	-DZCL_ENABLE_TIME_SERVER \
	-DXBEE_CELLULAR_ENABLED \
	-DXBEE_DEVICE_ENABLE_ATMODE \
	-DXBEE_XMODEM_TESTING \
	-DPOSIX
DEFINE ?= $(XBEE_DEFINES)
LIBS ?= -Lxbee/bin -lxbee
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic $(DEFINE) -I. -I./xbee/src
CFLAGS_DEPS ?= -MMD -MP $(CFLAGS)
OBJ = $(patsubst src/%.c,obj/%.o,$(wildcard src/*.c))
DEPS = $(OBJ:.o=.d)
LIB_DEPS = xbee\bin\libxbee.a

TARGET := airtight

.PHONY: all run install clean doc

all: bin/$(TARGET) $(OBJ)

obj/%.o: src/%.c
	@ mkdir -p obj
	$(CC) -c -o $@ $< $(CFLAGS_DEPS)

bin/$(TARGET): src/$(TARGET).c $(OBJ) $(LIB_DEPS)
	@ mkdir -p bin
	$(CC) $(CFLAGS) -o bin/$(TARGET) $(OBJ) $(LIBS)

xbee\bin\libxbee.a:
	cd xbee && $(MAKE)

run: bin/$(TARGET)
	./bin/$(TARGET) $(ARGS)

install: bin/$(TARGET)
	@ mkdir -p $(PREFIX)/bin
	cp bin/$^ $(PREFIX)/bin

doc:
	doxygen

clean:
	$(RM) $(OBJ)
	$(RM) $(DEPS)
	$(RM) bin/$(TARGET)
	cd xbee && $(MAKE) clean

-include $(DEPS)
