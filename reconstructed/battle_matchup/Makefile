CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_matchup.c ../battle_types/battle_types.c
HEADERS = battle_matchup.h ../battle_types/battle_types.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_matchup
build/test_battle_matchup: $(SOURCES) $(HEADERS) test_battle_matchup.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_matchup.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_matchup
	./build/test_battle_matchup
clean:
	rm -f build/test_battle_matchup
