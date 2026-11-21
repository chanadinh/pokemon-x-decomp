CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_tables_1.c ../battle_handler/battle_handler.c
HEADERS = battle_tables_1.h ../battle_handler/battle_handler.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_tables_1
build/test_battle_tables_1: $(SOURCES) $(HEADERS) test_battle_tables_1.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_tables_1.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_tables_1
	./build/test_battle_tables_1
clean:
	rm -f build/test_battle_tables_1
