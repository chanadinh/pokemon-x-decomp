CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = pokemon_names.c ../pokemon_learnset/pokemon_learnset.c ../pokemon_moves/pokemon_moves.c ../pokemon_state/pokemon_state.c ../pokemon_state/state_fields.c ../pokemon_data/pokemon_fields.c ../pokemon_data/pokemon_codec.c ../pokemon_constructor/pokemon_constructor.c ../pokemon_constructor/pokemon_parameters.c
HEADERS = pokemon_names.h ../pokemon_learnset/pokemon_learnset.h ../pokemon_moves/pokemon_moves.h ../pokemon_state/pokemon_state.h ../pokemon_state/state_fields.h ../pokemon_data/pokemon_data.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_pokemon_names

build/test_pokemon_names: $(SOURCES) $(HEADERS) test_pokemon_names.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_pokemon_names.c $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_pokemon_names
	./build/test_pokemon_names

clean:
	rm -f build/test_pokemon_names
