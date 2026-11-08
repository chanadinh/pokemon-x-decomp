CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_moves_4.c ../battle_modifier/battle_modifier.c ../battle_handler/battle_handler.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_moves_4.h ../battle_modifier/battle_modifier.h ../battle_handler/battle_handler.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_moves_4
build/test_battle_moves_4: $(SOURCES) $(HEADERS) test_battle_moves_4.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_moves_4.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_moves_4
	./build/test_battle_moves_4
clean:
	rm -f build/test_battle_moves_4
