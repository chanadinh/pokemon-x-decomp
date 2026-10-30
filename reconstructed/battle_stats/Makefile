CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_stats.c
HEADERS = battle_stats.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_stats
build/test_battle_stats: $(SOURCES) $(HEADERS) test_battle_stats.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_stats.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_stats
	./build/test_battle_stats
clean:
	rm -f build/test_battle_stats
