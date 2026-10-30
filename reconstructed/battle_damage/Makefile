CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_damage.c
HEADERS = battle_damage.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_damage
build/test_battle_damage: $(SOURCES) $(HEADERS) test_battle_damage.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_damage.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_damage
	./build/test_battle_damage
clean:
	rm -f build/test_battle_damage
