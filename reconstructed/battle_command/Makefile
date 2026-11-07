CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_command.c
HEADERS = battle_command.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_command
build/test_battle_command: $(SOURCES) $(HEADERS) test_battle_command.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_command.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_command
	./build/test_battle_command
clean:
	rm -f build/test_battle_command
