CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = pokemon_data.c pokemon_codec.c pokemon_fields.c test_pokemon_data.c test_pokemon_codec.c test_pokemon_fields.c test_pokemon_data_chain.c ../pokemon_constructor/pokemon_constructor.c ../pokemon_constructor/pokemon_parameters.c ../encounter_builder/encounter_builder.c ../battle_task/battle_task.c ../wild_battle_setup/wild_battle_setup.c ../call_wild_battle/call_wild_battle.c
HEADERS = pokemon_data.h ../pokemon_constructor/pokemon_constructor.h ../encounter_builder/encounter_builder.h ../battle_task/battle_task.h ../wild_battle_setup/wild_battle_setup.h ../call_wild_battle/call_wild_battle.h

.PHONY: all test clean
all: build/test_pokemon_data

build/test_pokemon_data: $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_pokemon_data
	./build/test_pokemon_data

clean:
	rm -f build/test_pokemon_data
