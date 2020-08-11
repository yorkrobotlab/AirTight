CC = gcc
PREFIX ?= /usr/local
DEFINE = \
	-DXBEE_PLATFORM_HEADER='"ports/posix/platform_config.h"' \
	-DZCL_ENABLE_TIME_SERVER \
	-DXBEE_CELLULAR_ENABLED \
	-DXBEE_DEVICE_ENABLE_ATMODE \
	-DXBEE_XMODEM_TESTING \
	-DPOSIX
CFLAGS ?= -std=gnu99 -g -Wall -I. -I./src $(DEFINE)
CFLAGS_DEPS ?= -MMD -MP $(CFLAGS)
OBJ = $(patsubst src/%.c,obj/%.o,$(wildcard src/*/*.c))
OBJ += $(patsubst src/%.c,obj/%.o,$(wildcard src/ports/posix/*.c))
DEPS = $(OBJ:.o=.d)

TARGET := libxbee.a

.PHONY: all run install clean

all: bin/$(TARGET) $(OBJ)

obj/%.o: src/%.c
	@ mkdir -p $(shell dirname $@)
	$(CC) -c -o $@ $< $(CFLAGS_DEPS)

bin/$(TARGET): $(OBJ)
	@ mkdir -p bin
	ar -scr bin/$(TARGET) $(OBJ)
	# $(CC) $(CFLAGS) -o bin/$(TARGET) $(OBJ)

run: bin/$(TARGET)
	./bin/$(TARGET) $(ARGS)

install: bin/$(TARGET)
	@ mkdir -p $(PREFIX)/bin
	cp bin/$^ $(PREFIX)/bin

clean:
	$(RM) $(OBJ)
	$(RM) $(DEPS)
	$(RM) bin/$(TARGET)

-include $(DEPS)
