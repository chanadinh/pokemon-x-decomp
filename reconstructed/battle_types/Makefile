CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_types.c
HEADERS = battle_types.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_types
build/test_battle_types: $(SOURCES) $(HEADERS) test_battle_types.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_types.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_types
	./build/test_battle_types
clean:
	rm -f build/test_battle_types
