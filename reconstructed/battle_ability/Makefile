CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_ability
build/test_battle_ability: $(SOURCES) $(HEADERS) test_battle_ability.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_ability.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_ability
	./build/test_battle_ability
clean:
	rm -f build/test_battle_ability
