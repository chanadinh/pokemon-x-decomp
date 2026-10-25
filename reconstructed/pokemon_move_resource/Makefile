CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = pokemon_move_resource.c ../pokemon_moves/pokemon_moves.c ../pokemon_state/state_fields.c ../pokemon_data/pokemon_codec.c ../pokemon_constructor/pokemon_constructor.c ../pokemon_constructor/pokemon_parameters.c
HEADERS = pokemon_move_resource.h ../pokemon_moves/pokemon_moves.h ../pokemon_state/state_fields.h ../pokemon_data/pokemon_data.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_pokemon_move_resource
build/test_pokemon_move_resource: $(SOURCES) $(HEADERS) test_pokemon_move_resource.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_pokemon_move_resource.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_pokemon_move_resource
	./build/test_pokemon_move_resource
clean:
	rm -f build/test_pokemon_move_resource
