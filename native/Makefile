CC = cc
PKG_CONFIG ?= pkg-config
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SDL_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags sdl2 2>/dev/null)
SDL_LIBS ?= $(shell $(PKG_CONFIG) --libs sdl2 2>/dev/null)

# The reconstructed HP and stat routines (and the data module they link
# against) run in the battle path.
RECON = ../reconstructed
STATS_SOURCES = $(RECON)/pokemon_stats/stats_math.c $(RECON)/pokemon_data/pokemon_data.c \
	$(RECON)/pokemon_data/pokemon_codec.c $(RECON)/pokemon_data/pokemon_fields.c \
	$(RECON)/pokemon_constructor/pokemon_constructor.c $(RECON)/pokemon_constructor/pokemon_parameters.c
ASSET_SOURCES = assets/garc.c assets/lz.c assets/bclim.c assets/etc1.c assets/texture.c assets/cgfx.c assets/bch_texture.c assets/field.c
ASSET_HEADERS = assets/garc.h assets/lz.h assets/bclim.h assets/etc1.h assets/texture.h assets/cgfx.h assets/bch_texture.h assets/field.h
MODEL_SOURCES = assets/cgfx_model.c assets/bch_model.c assets/bch_material.c assets/bch_texture.c assets/field.c assets/garc.c assets/lz.c assets/cgfx.c assets/texture.c assets/etc1.c render/raster.c render/tev.c render/light.c render/skin.c assets/gf_motion.c assets/zone.c assets/gf_script.c assets/gf_text.c assets/zone_talk.c render/text.c assets/pokedata.c game/battle.c game/battle_ui.c $(STATS_SOURCES)
MODEL_HEADERS = assets/cgfx_model.h assets/bch_model.h assets/bch_material.h assets/bch_texture.h assets/field.h assets/garc.h assets/lz.h assets/cgfx.h assets/texture.h assets/etc1.h render/raster.h render/tev.h render/light.h render/skin.h assets/bch_scene.h assets/gf_motion.h assets/zone.h assets/gf_script.h assets/gf_text.h assets/zone_talk.h render/text.h render/text_glyphs.inc assets/pokedata.h game/battle.h game/battle_ui.h
FIELD_SOURCES = assets/field.c assets/garc.c assets/lz.c
FIELD_HEADERS = assets/field.h assets/garc.h assets/lz.h
CORE_TESTS = build/test_garc build/test_lz build/test_bclim build/test_etc1 build/test_texture build/test_cgfx build/test_cgfx_model build/test_cgfx_scene build/test_raster build/test_field build/test_bch_texture build/test_bch_model build/test_bch_material build/test_tev build/test_bch_scene build/test_light build/test_skin build/test_gf_motion build/test_zone build/test_gf_text build/test_gf_script build/test_pokedata build/test_battle

.PHONY: all test test-core test-runtime clean check-sdl
all: build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer

check-sdl:
	@test -n "$(SDL_LIBS)" || { echo 'SDL2 not configured: install SDL2 development files or set SDL_CFLAGS and SDL_LIBS.' >&2; exit 1; }

build/pokemon_asset_viewer: app.c $(ASSET_SOURCES) $(ASSET_HEADERS) | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) app.c $(ASSET_SOURCES) $(LDFLAGS) $(SDL_LIBS) $(LDLIBS) -lm -o $@

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

build/test_bch_texture: assets/bch_texture.c assets/bch_texture.h assets/texture.c assets/texture.h assets/etc1.c assets/etc1.h assets/test_bch_texture.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/bch_texture.c assets/texture.c assets/etc1.c assets/test_bch_texture.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_bch_model: assets/bch_model.c assets/bch_model.h assets/test_bch_model.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/bch_model.c assets/test_bch_model.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_bch_scene: assets/bch_model.c assets/bch_model.h assets/bch_scene.h assets/test_bch_scene.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/bch_model.c assets/test_bch_scene.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_gf_text: assets/gf_text.c assets/gf_text.h assets/test_gf_text.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/gf_text.c assets/test_gf_text.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_gf_script: assets/gf_script.c assets/gf_script.h assets/zone_talk.c assets/zone_talk.h assets/test_gf_script.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/gf_script.c assets/zone_talk.c assets/test_gf_script.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_pokedata: assets/pokedata.c assets/pokedata.h assets/test_pokedata.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/pokedata.c assets/test_pokedata.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_battle: game/battle.c game/battle.h game/battle_ui.c game/battle_ui.h game/test_battle.c assets/pokedata.c render/text.c $(STATS_SOURCES) | build
	$(CC) $(CPPFLAGS) $(CFLAGS) game/battle.c game/battle_ui.c game/test_battle.c assets/pokedata.c render/text.c $(STATS_SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

build/test_zone: assets/zone.c assets/zone.h assets/test_zone.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/zone.c assets/test_zone.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_gf_motion: assets/gf_motion.c assets/gf_motion.h assets/test_gf_motion.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -D_DEFAULT_SOURCE assets/gf_motion.c assets/test_gf_motion.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_skin: render/skin.c render/skin.h assets/bch_model.h assets/gf_motion.c assets/gf_motion.h render/test_skin.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -D_DEFAULT_SOURCE render/skin.c assets/gf_motion.c render/test_skin.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_light: render/light.c render/light.h render/test_light.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) render/light.c render/test_light.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_bch_material: assets/bch_material.c assets/bch_material.h assets/bch_model.h render/tev.c render/tev.h assets/test_bch_material.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) assets/bch_material.c render/tev.c assets/test_bch_material.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_tev: render/tev.c render/tev.h render/test_tev.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) render/tev.c render/test_tev.c $(LDFLAGS) $(LDLIBS) -o $@

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
	./build/test_bch_texture
	./build/test_bch_model
	./build/test_bch_material
	./build/test_tev
	./build/test_bch_scene
	./build/test_light
	./build/test_skin
	./build/test_gf_motion
	./build/test_zone
	./build/test_gf_text
	./build/test_gf_script
	./build/test_pokedata
	./build/test_battle
	python3 -m unittest discover -s tests -p test_container.py -v
	python3 -m unittest discover -s tests -p test_bch.py -v
	python3 -m unittest discover -s tests -p test_pica_commands.py -v
	python3 -m unittest discover -s tests -p test_bch_model.py -v

test-runtime: build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer
	./build/pokemon_asset_viewer --demo --inspect
	SDL_VIDEODRIVER=dummy ./build/pokemon_asset_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_viewer.py -v
	SDL_VIDEODRIVER=dummy ./build/pokemon_model_viewer --demo --software --frames 3 --self-test-input
	SDL_VIDEODRIVER=dummy ./build/pokemon_model_viewer --demo --textured --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_model_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_model_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_textured_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_combiner_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_lighting_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_walk_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_player_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_motion_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_zone_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_talk_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_town_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_battle_viewer.py -v
	SDL_VIDEODRIVER=dummy ./build/pokemon_field_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_field_viewer.py -v

clean:
	rm -f build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer $(CORE_TESTS)
