CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_message.c ../battle_command/battle_command.c
HEADERS = battle_message.h ../battle_command/battle_command.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_message
build/test_battle_message: $(SOURCES) $(HEADERS) test_battle_message.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_message.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_message
	./build/test_battle_message
clean:
	rm -f build/test_battle_message
