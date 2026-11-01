CC ?= cc
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -pedantic
ROOT := ..
SOURCES := physics_test.c $(wildcard $(ROOT)/source/game/*.c)
HEADERS := $(wildcard $(ROOT)/include/*.h) $(ROOT)/assets/generated/movement_level.h
OUTPUT := $(ROOT)/build/host/physics_tests
ORIENTATION := $(ROOT)/build/host/orientation_tests
RENDER := $(ROOT)/build/host/render_tests
MOVEMENT := $(ROOT)/build/host/movement_v04_tests
CORRECTIONS := $(ROOT)/build/host/corrections_tests
.PHONY: test clean
test: $(OUTPUT) $(ORIENTATION) $(RENDER) $(MOVEMENT) $(CORRECTIONS)
	$(OUTPUT)
	$(ORIENTATION)
	$(RENDER)
	$(MOVEMENT)
	$(CORRECTIONS)
$(OUTPUT): $(SOURCES) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated $(SOURCES) -lm -o $@
$(ORIENTATION): orientation_test.c $(ROOT)/source/render/player_sprite.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS) $(ROOT)/include/player_sprite.h $(ROOT)/assets/generated/mario_atlas.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated orientation_test.c $(ROOT)/source/render/player_sprite.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
$(RENDER): $(wildcard $(ROOT)/source/game/*.c) render_test.c $(ROOT)/source/platform/render.c $(ROOT)/source/render/player_sprite.c $(HEADERS) $(ROOT)/include/player_sprite.h $(ROOT)/include/render.h $(ROOT)/assets/generated/mario_atlas.h stubs/graphics.h stubs/mario_rgba8_bin.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Istubs -I$(ROOT)/include -I$(ROOT)/assets/generated render_test.c $(ROOT)/source/platform/render.c $(ROOT)/source/render/player_sprite.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
$(MOVEMENT): movement_v04_test.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated movement_v04_test.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
$(CORRECTIONS): corrections_test.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated corrections_test.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
clean:
	rm -f $(OUTPUT) $(ORIENTATION) $(RENDER) $(MOVEMENT) $(CORRECTIONS)
