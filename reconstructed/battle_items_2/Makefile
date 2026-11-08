CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_items_2.c ../battle_items_1/battle_items_1.c ../battle_abilities_1/battle_abilities_1.c ../battle_modifier/battle_modifier.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_items_2.h ../battle_items_1/battle_items_1.h ../battle_abilities_1/battle_abilities_1.h ../battle_modifier/battle_modifier.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_items_2
build/test_battle_items_2: $(SOURCES) $(HEADERS) test_battle_items_2.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_items_2.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_items_2
	./build/test_battle_items_2
clean:
	rm -f build/test_battle_items_2
