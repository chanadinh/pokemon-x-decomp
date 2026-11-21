CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_mid_3.c
HEADERS = battle_mid_3.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_mid_3
build/test_battle_mid_3: $(SOURCES) $(HEADERS) test_battle_mid_3.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_mid_3.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_mid_3
	./build/test_battle_mid_3
clean:
	rm -f build/test_battle_mid_3
