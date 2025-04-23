CC      := gcc
CFLAGS  := -Wall -fPIC -O2 -D_GNU_SOURCE -Iinclude
LDFLAGS := -shared
BUILD   := build

.PHONY: all clean

all: $(BUILD)/libzpoline.so.1

$(BUILD)/libzpoline.so.1: src/init_trampoline.c src/trampoline_hello.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)