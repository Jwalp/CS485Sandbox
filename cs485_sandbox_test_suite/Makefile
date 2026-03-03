CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
INCLUDES := -Iinclude

SRC := $(shell find public ta_hidden -name "*.c")
BINS := $(patsubst %.c,bin/%,$(SRC))

all: $(BINS)

bin/%: %.c include/common.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf bin

.PHONY: all clean
