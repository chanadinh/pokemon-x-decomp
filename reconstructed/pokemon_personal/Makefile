CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = pokemon_personal.c ../pokemon_learnset/pokemon_learnset.c
HEADERS = pokemon_personal.h ../pokemon_learnset/pokemon_learnset.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_pokemon_personal
build/test_pokemon_personal: $(SOURCES) $(HEADERS) test_pokemon_personal.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_pokemon_personal.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_pokemon_personal
	./build/test_pokemon_personal
clean:
	rm -f build/test_pokemon_personal
