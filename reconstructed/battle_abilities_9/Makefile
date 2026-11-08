CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_abilities_9.c ../battle_moves_3/battle_moves_3.c ../battle_items_4/battle_items_4.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_abilities_9.h ../battle_moves_3/battle_moves_3.h ../battle_items_4/battle_items_4.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_abilities_9
build/test_battle_abilities_9: $(SOURCES) $(HEADERS) test_battle_abilities_9.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_abilities_9.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_abilities_9
	./build/test_battle_abilities_9
clean:
	rm -f build/test_battle_abilities_9
