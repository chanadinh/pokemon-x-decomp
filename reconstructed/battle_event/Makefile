CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_event.c
HEADERS = battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_event
build/test_battle_event: $(SOURCES) $(HEADERS) test_battle_event.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_event.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_event
	./build/test_battle_event
clean:
	rm -f build/test_battle_event
