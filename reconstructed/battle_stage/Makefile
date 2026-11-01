CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_stage.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_stage.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_stage
build/test_battle_stage: $(SOURCES) $(HEADERS) test_battle_stage.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_stage.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_stage
	./build/test_battle_stage
clean:
	rm -f build/test_battle_stage
