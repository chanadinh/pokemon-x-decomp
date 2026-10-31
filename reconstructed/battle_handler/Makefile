CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_handler.c
HEADERS = battle_handler.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_handler
build/test_battle_handler: $(SOURCES) $(HEADERS) test_battle_handler.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_handler.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_handler
	./build/test_battle_handler
clean:
	rm -f build/test_battle_handler
