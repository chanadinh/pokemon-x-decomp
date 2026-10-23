CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = pokemon_constructor.c pokemon_parameters.c test_pokemon_constructor.c test_pokemon_parameters.c test_pokemon_constructor_chain.c ../encounter_builder/encounter_builder.c ../battle_task/battle_task.c ../wild_battle_setup/wild_battle_setup.c ../call_wild_battle/call_wild_battle.c
HEADERS = pokemon_constructor.h ../encounter_builder/encounter_builder.h ../battle_task/battle_task.h ../wild_battle_setup/wild_battle_setup.h ../call_wild_battle/call_wild_battle.h

.PHONY: all test clean
all: build/test_pokemon_constructor

build/test_pokemon_constructor: $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_pokemon_constructor
	./build/test_pokemon_constructor

clean:
	rm -f build/test_pokemon_constructor
