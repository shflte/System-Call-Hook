CC       := gcc
AS       := gcc
BUILD    := build
SRC      := $(wildcard src/*.c) $(wildcard src/*.S)
OBJ      := $(patsubst src/%.c,$(BUILD)/%.o,$(filter %.c,$(SRC))) \
            $(patsubst src/%.S,$(BUILD)/%.o,$(filter %.S,$(SRC)))
INCLUDE  := -Iinclude -I../common
BASE_CFLAGS := -Wall -fPIC -D_GNU_SOURCE $(INCLUDE)
ASFLAGS  := -Wall -fPIC -masm=intel
LDFLAGS  := -shared
LDLIBS   := -lcapstone -ldl

ifeq ($(debug),1)
    CFLAGS := $(BASE_CFLAGS) -O0 -g
else
    CFLAGS := $(BASE_CFLAGS) -O2
endif

.PHONY: all clean

all: $(BUILD)/libzpoline.so

$(BUILD)/libzpoline.so: $(OBJ)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.S | $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)