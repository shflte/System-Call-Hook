CC = gcc
AS = gcc
BUILD_DIR = build
SRC_DIR = src

LIBS = libzpoline.so.1 libzpoline.so.2

# Default to optimized build
OPT_FLAGS = -O2
DEBUG_FLAGS = -O0 -g

CFLAGS = -Wall -fPIC -D_GNU_SOURCE -Iinclude
ASFLAGS = -Wall -fPIC -masm=intel
LDFLAGS = -shared
LDLIBS = -lcapstone

ifeq ($(debug),1)
	CFLAGS += $(DEBUG_FLAGS)
else
	CFLAGS += $(OPT_FLAGS)
endif

.PHONY: all clean

all: $(addprefix $(BUILD_DIR)/, $(LIBS))

# Step 1
$(BUILD_DIR)/libzpoline.so.1: \
	$(BUILD_DIR)/init_trampoline.o \
	$(BUILD_DIR)/trampoline_hello.o
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# Step 2
$(BUILD_DIR)/libzpoline.so.2: \
	$(BUILD_DIR)/init_trampoline.o \
	$(BUILD_DIR)/rewrite_syscall.o \
	$(BUILD_DIR)/syscall_hook_handler.o \
	$(BUILD_DIR)/trampoline_entry.o \
	$(BUILD_DIR)/trigger_syscall.o
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)