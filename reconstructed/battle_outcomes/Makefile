CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_outcomes.c ../battle_process/battle_process.c ../battle_action/battle_action.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_outcomes.h ../battle_process/battle_process.h ../battle_action/battle_action.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_outcomes
build/test_battle_outcomes: $(SOURCES) $(HEADERS) test_battle_outcomes.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_outcomes.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_outcomes
	./build/test_battle_outcomes
clean:
	rm -f build/test_battle_outcomes
