CC = gcc
CFLAGS = -Wall -fPIC -O2 -D_GNU_SOURCE -Iinclude
LDFLAGS = -shared

SRC_DIR = src
BUILD_DIR = build

LIB_NAME = libzpoline.so.1
SRC_FILE = $(SRC_DIR)/trampoline.c
OUT_FILE = $(BUILD_DIR)/$(LIB_NAME)

.PHONY: all clean

all: $(OUT_FILE)

$(OUT_FILE): $(SRC_FILE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)