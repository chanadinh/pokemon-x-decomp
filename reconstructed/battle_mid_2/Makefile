CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_mid_2.c
HEADERS = battle_mid_2.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_mid_2
build/test_battle_mid_2: $(SOURCES) $(HEADERS) test_battle_mid_2.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_mid_2.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_mid_2
	./build/test_battle_mid_2
clean:
	rm -f build/test_battle_mid_2
