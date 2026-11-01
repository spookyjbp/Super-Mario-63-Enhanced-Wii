CC ?= cc
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -pedantic
ROOT := ..
SOURCES := physics_test.c $(wildcard $(ROOT)/source/game/*.c)
HEADERS := $(ROOT)/include/game.h
OUTPUT := $(ROOT)/build/host/physics_tests
.PHONY: test clean
test: $(OUTPUT)
	$(OUTPUT)
$(OUTPUT): $(SOURCES) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include $(SOURCES) -lm -o $@
clean:
	rm -f $(OUTPUT)
