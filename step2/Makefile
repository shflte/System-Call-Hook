CC       := gcc
AS       := gcc
BUILD    := build
SRC_DIR  := src
TARGET   := libzpoline.so.2
OBJS     := init_trampoline.o rewrite_syscall.o syscall_hook_handler.o \
            trampoline_entry.o trigger_syscall.o

CFLAGS   := -Wall -fPIC -D_GNU_SOURCE -Iinclude
ASFLAGS  := -Wall -fPIC -masm=intel
LDFLAGS  := -shared
LDLIBS   := -lcapstone

.PHONY: all clean

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(addprefix $(BUILD)/, $(OBJS))
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.S | $(BUILD)
	$(AS) $(ASFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)