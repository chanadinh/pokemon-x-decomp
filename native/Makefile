CC = cc
PKG_CONFIG ?= pkg-config
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SDL_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags sdl2 2>/dev/null)
SDL_LIBS ?= $(shell $(PKG_CONFIG) --libs sdl2 2>/dev/null)

ASSET_SOURCES = assets/garc.c assets/lz.c assets/bclim.c assets/etc1.c assets/texture.c assets/cgfx.c
ASSET_HEADERS = assets/garc.h assets/lz.h assets/bclim.h assets/etc1.h assets/texture.h assets/cgfx.h
MODEL_SOURCES = assets/cgfx_model.c assets/garc.c assets/lz.c assets/cgfx.c assets/texture.c assets/etc1.c render/raster.c
MODEL_HEADERS = assets/cgfx_model.h assets/garc.h assets/lz.h assets/cgfx.h assets/texture.h assets/etc1.h render/raster.h
FIELD_SOURCES = assets/field.c assets/garc.c assets/lz.c
FIELD_HEADERS = assets/field.h assets/garc.h assets/lz.h
CORE_TESTS = build/test_garc build/test_lz build/test_bclim build/test_etc1 build/test_texture build/test_cgfx build/test_cgfx_model build/test_cgfx_scene build/test_raster build/test_field

.PHONY: all test test-core test-runtime clean check-sdl
all: build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer

check-sdl:
	@test -n "$(SDL_LIBS)" || { echo 'SDL2 not configured: install SDL2 development files or set SDL_CFLAGS and SDL_LIBS.' >&2; exit 1; }

build/pokemon_asset_viewer: app.c $(ASSET_SOURCES) $(ASSET_HEADERS) | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) app.c $(ASSET_SOURCES) $(LDFLAGS) $(SDL_LIBS) $(LDLIBS) -o $@

build/pokemon_model_viewer: model_app.c $(MODEL_SOURCES) $(MODEL_HEADERS) | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) model_app.c $(MODEL_SOURCES) $(LDFLAGS) $(SDL_LIBS) $(LDLIBS) -lm -o $@

build/pokemon_field_viewer: field_app.c $(FIELD_SOURCES) $(FIELD_HEADERS) | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) field_app.c $(FIELD_SOURCES) $(LDFLAGS) $(SDL_LIBS) $(LDLIBS) -lm -o $@

build/test_field: assets/field.c assets/field.h assets/test_field.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/field.c assets/test_field.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_garc: assets/garc.c assets/garc.h assets/test_garc.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/garc.c assets/test_garc.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_lz: assets/lz.c assets/lz.h assets/test_lz.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/lz.c assets/test_lz.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_bclim: assets/bclim.c assets/bclim.h assets/etc1.c assets/etc1.h assets/texture.c assets/texture.h assets/test_bclim.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/bclim.c assets/texture.c assets/etc1.c assets/test_bclim.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_etc1: assets/etc1.c assets/etc1.h assets/test_etc1.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/etc1.c assets/test_etc1.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_texture: assets/texture.c assets/texture.h assets/etc1.c assets/etc1.h assets/test_texture.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/texture.c assets/etc1.c assets/test_texture.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_cgfx: assets/cgfx.c assets/cgfx.h assets/texture.c assets/texture.h assets/etc1.c assets/etc1.h assets/test_cgfx.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/cgfx.c assets/texture.c assets/etc1.c assets/test_cgfx.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_cgfx_model: assets/cgfx_model.c assets/cgfx_model.h assets/test_cgfx_model.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/cgfx_model.c assets/test_cgfx_model.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_cgfx_scene: assets/cgfx_model.c assets/cgfx_model.h assets/test_cgfx_scene.c assets/cgfx.c assets/cgfx.h assets/texture.c assets/texture.h assets/etc1.c assets/etc1.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/cgfx_model.c assets/cgfx.c assets/texture.c assets/etc1.c assets/test_cgfx_scene.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_raster: render/raster.c render/raster.h render/test_raster.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) render/raster.c render/test_raster.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build:
	mkdir -p build

test test-core: $(CORE_TESTS)
	./build/test_garc
	./build/test_lz
	./build/test_bclim
	./build/test_etc1
	./build/test_texture
	./build/test_cgfx
	./build/test_cgfx_model
	./build/test_cgfx_scene
	./build/test_raster
	./build/test_field
	python3 -m unittest discover -s tests -p test_container.py -v

test-runtime: build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer
	./build/pokemon_asset_viewer --demo --inspect
	SDL_VIDEODRIVER=dummy ./build/pokemon_asset_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_viewer.py -v
	SDL_VIDEODRIVER=dummy ./build/pokemon_model_viewer --demo --software --frames 3 --self-test-input
	SDL_VIDEODRIVER=dummy ./build/pokemon_model_viewer --demo --textured --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_model_viewer.py -v
	SDL_VIDEODRIVER=dummy ./build/pokemon_field_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_field_viewer.py -v

clean:
	rm -f build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer $(CORE_TESTS)
