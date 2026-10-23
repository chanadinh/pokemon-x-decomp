CC = cc
PKG_CONFIG ?= pkg-config
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SDL_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags sdl2 2>/dev/null)
SDL_LIBS ?= $(shell $(PKG_CONFIG) --libs sdl2 2>/dev/null)

ASSET_SOURCES = assets/garc.c assets/lz.c assets/bclim.c assets/etc1.c assets/texture.c assets/cgfx.c
ASSET_HEADERS = assets/garc.h assets/lz.h assets/bclim.h assets/etc1.h assets/texture.h assets/cgfx.h
CORE_TESTS = build/test_garc build/test_lz build/test_bclim build/test_etc1 build/test_texture build/test_cgfx build/test_cgfx_model

.PHONY: all test test-core test-runtime clean check-sdl
all: build/pokemon_asset_viewer build/pokemon_model_viewer

check-sdl:
	@test -n "$(SDL_LIBS)" || { echo 'SDL2 not configured: install SDL2 development files or set SDL_CFLAGS and SDL_LIBS.' >&2; exit 1; }

build/pokemon_asset_viewer: app.c $(ASSET_SOURCES) $(ASSET_HEADERS) | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) app.c $(ASSET_SOURCES) $(LDFLAGS) $(SDL_LIBS) $(LDLIBS) -o $@

build/pokemon_model_viewer: model_app.c assets/cgfx_model.c assets/cgfx_model.h assets/garc.c assets/garc.h assets/lz.c assets/lz.h | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) model_app.c assets/cgfx_model.c assets/garc.c assets/lz.c $(LDFLAGS) $(SDL_LIBS) $(LDLIBS) -lm -o $@

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
	python3 -m unittest discover -s tests -p test_container.py -v

test-runtime: build/pokemon_asset_viewer build/pokemon_model_viewer
	./build/pokemon_asset_viewer --demo --inspect
	SDL_VIDEODRIVER=dummy ./build/pokemon_asset_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_viewer.py -v
	SDL_VIDEODRIVER=dummy ./build/pokemon_model_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_model_viewer.py -v

clean:
	rm -f build/pokemon_asset_viewer build/pokemon_model_viewer $(CORE_TESTS)
