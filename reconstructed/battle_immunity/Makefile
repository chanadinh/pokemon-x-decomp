CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_immunity.c ../battle_modifier/battle_modifier.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_immunity.h ../battle_modifier/battle_modifier.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_immunity
build/test_battle_immunity: $(SOURCES) $(HEADERS) test_battle_immunity.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_immunity.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_immunity
	./build/test_battle_immunity
clean:
	rm -f build/test_battle_immunity
