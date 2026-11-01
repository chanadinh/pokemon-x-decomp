CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_typechange.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_typechange.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_typechange
build/test_battle_typechange: $(SOURCES) $(HEADERS) test_battle_typechange.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_typechange.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_typechange
	./build/test_battle_typechange
clean:
	rm -f build/test_battle_typechange
