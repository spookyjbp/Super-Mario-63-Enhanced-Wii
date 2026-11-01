CC ?= cc
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -pedantic
ROOT := ..
SOURCES := physics_test.c $(wildcard $(ROOT)/source/game/*.c)
HEADERS := $(ROOT)/include/game.h
OUTPUT := $(ROOT)/build/host/physics_tests
ORIENTATION := $(ROOT)/build/host/orientation_tests
.PHONY: test clean
test: $(OUTPUT) $(ORIENTATION)
	$(OUTPUT)
	$(ORIENTATION)
$(OUTPUT): $(SOURCES) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include $(SOURCES) -lm -o $@
$(ORIENTATION): orientation_test.c $(ROOT)/source/render/player_sprite.c $(ROOT)/source/game/player.c $(HEADERS) $(ROOT)/include/player_sprite.h $(ROOT)/assets/generated/mario_atlas.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated orientation_test.c $(ROOT)/source/render/player_sprite.c $(ROOT)/source/game/player.c -lm -o $@
clean:
	rm -f $(OUTPUT) $(ORIENTATION)
