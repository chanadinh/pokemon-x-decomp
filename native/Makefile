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
# The platform layer (platform/): guest memory is part of every build that runs reconstructed code.
GAME_CORE = $(STATS_SOURCES) platform/guest.c platform/fs.c
PLATFORM_SOURCES = platform/hid.c platform/clock.c platform/thread_sdl.c platform/audio.c platform/audio_sdl.c
ASSET_SOURCES = assets/garc.c assets/lz.c assets/bclim.c assets/etc1.c assets/texture.c assets/cgfx.c assets/bch_texture.c assets/field.c
ASSET_HEADERS = assets/garc.h assets/lz.h assets/bclim.h assets/etc1.h assets/texture.h assets/cgfx.h assets/bch_texture.h assets/field.h
MODEL_SOURCES = assets/cgfx_model.c assets/bch_model.c assets/bch_material.c assets/bch_texture.c assets/field.c assets/garc.c assets/lz.c assets/cgfx.c assets/texture.c assets/etc1.c render/raster.c render/tev.c render/light.c render/skin.c assets/gf_motion.c assets/zone.c assets/gf_script.c assets/gf_text.c assets/zone_talk.c render/text.c assets/pokedata.c game/battle.c game/battle_ui.c game/save.c $(GAME_CORE) $(PLATFORM_SOURCES)
MODEL_HEADERS = platform/audio.h platform/audio_sdl.h platform/guest.h platform/fs.h platform/hid.h platform/clock.h platform/thread.h assets/cgfx_model.h assets/bch_model.h assets/bch_material.h assets/bch_texture.h assets/field.h assets/garc.h assets/lz.h assets/cgfx.h assets/texture.h assets/etc1.h render/raster.h render/tev.h render/light.h render/skin.h assets/bch_scene.h assets/gf_motion.h assets/zone.h assets/gf_script.h assets/gf_text.h assets/zone_talk.h render/text.h render/text_glyphs.inc assets/pokedata.h game/battle.h game/battle_ui.h game/save.h
FIELD_SOURCES = assets/field.c assets/garc.c assets/lz.c
FIELD_HEADERS = assets/field.h assets/garc.h assets/lz.h
CORE_TESTS = build/test_garc build/test_lz build/test_bclim build/test_etc1 build/test_texture build/test_cgfx build/test_cgfx_model build/test_cgfx_scene build/test_raster build/test_field build/test_bch_texture build/test_bch_model build/test_bch_material build/test_tev build/test_bch_scene build/test_light build/test_skin build/test_gf_motion build/test_zone build/test_gf_text build/test_gf_script build/test_pokedata build/test_battle build/test_save build/test_guest build/test_fs build/test_hid_clock build/test_audio build/test_pica_shader build/test_vertex_program

.PHONY: all test test-core test-runtime clean check-sdl
all: build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer build/pica_probe build/shader_check

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

build/test_battle: game/battle.c game/battle.h game/battle_ui.c game/battle_ui.h game/test_battle.c assets/pokedata.c render/text.c $(GAME_CORE) | build
	$(CC) $(CPPFLAGS) $(CFLAGS) game/battle.c game/battle_ui.c game/test_battle.c assets/pokedata.c render/text.c $(GAME_CORE) $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_guest: platform/guest.c platform/guest.h platform/test_guest.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) platform/guest.c platform/test_guest.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_fs: platform/fs.c platform/fs.h platform/test_fs.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) platform/fs.c platform/test_fs.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_hid_clock: platform/hid.c platform/hid.h platform/clock.c platform/clock.h platform/test_hid_clock.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) platform/hid.c platform/clock.c platform/test_hid_clock.c $(LDFLAGS) $(LDLIBS) -o $@

build/test_thread: platform/thread_sdl.c platform/thread.h platform/test_thread.c | build check-sdl
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SDL_CFLAGS) platform/thread_sdl.c platform/test_thread.c $(LDFLAGS) $(SDL_LIBS) -o $@

build/shader_check: tools/shader_check.c assets/bch_model.c assets/bch_model.h render/skin.c render/skin.h render/vertex_program.c render/vertex_program.h platform/pica_shader.c platform/pica_shader.h assets/gf_motion.c assets/gf_motion.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -D_DEFAULT_SOURCE tools/shader_check.c assets/bch_model.c render/skin.c render/vertex_program.c platform/pica_shader.c assets/gf_motion.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_vertex_program: render/vertex_program.c render/vertex_program.h render/test_vertex_program.c assets/bch_model.h platform/pica_shader.c platform/pica_shader.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) render/vertex_program.c platform/pica_shader.c render/test_vertex_program.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/pica_probe: platform/pica_shader.c platform/pica_shader.h platform/pica_probe.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) platform/pica_shader.c platform/pica_probe.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_pica_shader: platform/pica_shader.c platform/pica_shader.h platform/test_pica_shader.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) platform/pica_shader.c platform/test_pica_shader.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_audio: platform/audio.c platform/audio.h platform/test_audio.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) platform/audio.c platform/test_audio.c $(LDFLAGS) $(LDLIBS) -lm -o $@

build/test_save: game/save.c game/save.h game/test_save.c game/battle.c game/battle.h assets/pokedata.c $(GAME_CORE) | build
	$(CC) $(CPPFLAGS) $(CFLAGS) game/save.c game/test_save.c game/battle.c assets/pokedata.c $(GAME_CORE) $(LDFLAGS) $(LDLIBS) -lm -o $@

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
	./build/test_save
	./build/test_guest
	mkdir -p build/fs-test && ./build/test_fs build/fs-test
	./build/test_hid_clock
	./build/test_audio
	./build/test_pica_shader
	./build/test_vertex_program
	python3 -m unittest discover -s tests -p test_container.py -v
	python3 -m unittest discover -s tests -p test_bch.py -v
	python3 -m unittest discover -s tests -p test_pica_commands.py -v
	python3 -m unittest discover -s tests -p test_bch_model.py -v

test-runtime: build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer build/test_thread
	./build/test_thread
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
	python3 -m unittest discover -s tests -p test_bch_save_viewer.py -v
	python3 -m unittest discover -s tests -p test_bch_platform_viewer.py -v
	SDL_VIDEODRIVER=dummy ./build/pokemon_field_viewer --demo --software --frames 3 --self-test-input
	python3 -m unittest discover -s tests -p test_field_viewer.py -v

clean:
	rm -f build/pokemon_asset_viewer build/pokemon_model_viewer build/pokemon_field_viewer $(CORE_TESTS)
