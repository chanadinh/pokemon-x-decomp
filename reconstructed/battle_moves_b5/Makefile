CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_moves_b5.c ../battle_moves_5/battle_moves_5.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_moves_b5.h ../battle_moves_5/battle_moves_5.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_moves_b5
build/test_battle_moves_b5: $(SOURCES) $(HEADERS) test_battle_moves_b5.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_moves_b5.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_moves_b5
	./build/test_battle_moves_b5
clean:
	rm -f build/test_battle_moves_b5
