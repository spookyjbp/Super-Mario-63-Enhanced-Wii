CC ?= cc
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -pedantic
ROOT := ..
SOURCES := physics_test.c $(wildcard $(ROOT)/source/game/*.c)
HEADERS := $(ROOT)/include/game.h
OUTPUT := $(ROOT)/build/host/physics_tests
ORIENTATION := $(ROOT)/build/host/orientation_tests
RENDER := $(ROOT)/build/host/render_tests
.PHONY: test clean
test: $(OUTPUT) $(ORIENTATION) $(RENDER)
	$(OUTPUT)
	$(ORIENTATION)
	$(RENDER)
$(OUTPUT): $(SOURCES) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include $(SOURCES) -lm -o $@
$(ORIENTATION): orientation_test.c $(ROOT)/source/render/player_sprite.c $(ROOT)/source/game/player.c $(HEADERS) $(ROOT)/include/player_sprite.h $(ROOT)/assets/generated/mario_atlas.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated orientation_test.c $(ROOT)/source/render/player_sprite.c $(ROOT)/source/game/player.c -lm -o $@
$(RENDER): render_test.c $(ROOT)/source/platform/render.c $(ROOT)/source/render/player_sprite.c $(HEADERS) $(ROOT)/include/player_sprite.h $(ROOT)/include/render.h $(ROOT)/assets/generated/mario_atlas.h stubs/graphics.h stubs/mario_rgba8_bin.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Istubs -I$(ROOT)/include -I$(ROOT)/assets/generated render_test.c $(ROOT)/source/platform/render.c $(ROOT)/source/render/player_sprite.c -lm -o $@
clean:
	rm -f $(OUTPUT) $(ORIENTATION) $(RENDER)
