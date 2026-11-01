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
ART := $(ROOT)/build/host/art_pack_tests
CONTROLS := $(ROOT)/build/host/controls_tests
COURSE := $(ROOT)/build/host/course_pack_tests
PLAY := $(ROOT)/build/host/course_play_tests
SESSION := $(ROOT)/build/host/course_session_tests
CART := $(ROOT)/build/host/course_art_tests
OBJS := $(ROOT)/build/host/course_objects_tests
.PHONY: test clean
test: $(OUTPUT) $(ORIENTATION) $(RENDER) $(MOVEMENT) $(CORRECTIONS) $(CONTROLS) $(COURSE) $(PLAY) $(SESSION) $(CART) $(OBJS) $(ART)
	$(OUTPUT)
	$(ORIENTATION)
	$(RENDER)
	$(MOVEMENT)
	$(CORRECTIONS)
	$(CONTROLS)
	cd $(ROOT)/build/host && ./art_pack_tests
	cd $(ROOT)/build/host && ./course_pack_tests
	cd $(ROOT)/build/host && ./course_play_tests
	cd $(ROOT)/build/host && ./course_session_tests
	cd $(ROOT)/build/host && ./course_art_tests
	cd $(ROOT)/build/host && ./course_objects_tests
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
$(ART): art_pack_test.c $(ROOT)/source/assets/art_pack.c $(ROOT)/source/platform/user_art.c $(ROOT)/source/render/player_art_select.c $(HEADERS) stubs/fat.h stubs/graphics.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Istubs -I$(ROOT)/include art_pack_test.c $(ROOT)/source/assets/art_pack.c $(ROOT)/source/platform/user_art.c $(ROOT)/source/render/player_art_select.c $(wildcard $(ROOT)/source/game/*.c) -I$(ROOT)/assets/generated -lm -o $@
$(CONTROLS): controls_test.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated controls_test.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
$(COURSE): course_pack_test.c $(ROOT)/source/game/course_pack.c $(ROOT)/include/course_pack.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include course_pack_test.c $(ROOT)/source/game/course_pack.c -lm -o $@
$(PLAY): course_play_test.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated course_play_test.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
$(SESSION): course_session_test.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated course_session_test.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
$(CART): course_art_test.c $(ROOT)/source/assets/course_art.c $(ROOT)/source/assets/lz4_block.c $(ROOT)/source/game/course_pack.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include course_art_test.c $(ROOT)/source/assets/course_art.c $(ROOT)/source/assets/lz4_block.c $(ROOT)/source/game/course_pack.c -lm -o $@
$(OBJS): course_objects_test.c $(wildcard $(ROOT)/source/game/*.c) $(HEADERS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(ROOT)/include -I$(ROOT)/assets/generated course_objects_test.c $(wildcard $(ROOT)/source/game/*.c) -lm -o $@
clean:
	rm -f $(OUTPUT) $(ORIENTATION) $(RENDER) $(MOVEMENT) $(CORRECTIONS) $(CONTROLS) $(COURSE) $(PLAY) $(SESSION) $(CART) $(OBJS) $(ART)
