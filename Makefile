CC       := gcc
AS       := gcc
BUILD    := build
SRC      := $(wildcard src/*.c) $(wildcard src/*.S)
OBJ	     := $(patsubst src/%.c,$(BUILD)/%.o,$(filter %.c,$(SRC))) \
            $(patsubst src/%.S,$(BUILD)/%.o,$(filter %.S,$(SRC)))
INCLUDE := -Iinclude
BASE_CFLAGS := -Wall -fPIC -D_GNU_SOURCE $(INCLUDE)
ASFLAGS := -Wall -fPIC -masm=intel
LDFLAGS := -shared
LDLIBS  := -lcapstone -ldl

ifeq ($(debug),1)
    CFLAGS := $(BASE_CFLAGS) -O0 -g
else
    CFLAGS := $(BASE_CFLAGS) -O2
endif

.PHONY: all step1 step2 copy_so clean

all: step1 step2 $(BUILD)/libzpoline.so copy_so

step1:
	@$(MAKE) -C step1

step2:
	@$(MAKE) -C step2

$(BUILD)/libzpoline.so: $(OBJ)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: src/%.S | $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

copy_so: step1 step2 $(BUILD)/libzpoline.so
	@cp step1/build/libzpoline.so.1   .
	@cp step2/build/libzpoline.so.2   .
	@cp build/libzpoline.so           .

clean:
	@$(MAKE) -C step1 clean
	@$(MAKE) -C step2 clean
	rm -rf $(BUILD) libzpoline.so.1 libzpoline.so.2 libzpoline.so