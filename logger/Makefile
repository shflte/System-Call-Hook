CC       := gcc
BUILD    := build
SRC      := $(wildcard src/*.c)
OBJ      := $(patsubst src/%.c,$(BUILD)/%.o,$(SRC))
INCLUDE  := -Iinclude -I../common
BASE_CFLAGS := -Wall -fPIC -D_GNU_SOURCE $(INCLUDE)
LDFLAGS  := -shared
TARGET   := $(BUILD)/logger.so

ifeq ($(debug),1)
    CFLAGS := $(BASE_CFLAGS) -O0 -g
else
    CFLAGS := $(BASE_CFLAGS) -O2
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)