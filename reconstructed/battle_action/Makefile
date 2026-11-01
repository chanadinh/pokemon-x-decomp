CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_action.c ../battle_insight/battle_insight.c ../battle_ability/battle_ability.c ../battle_event/battle_event.c
HEADERS = battle_action.h ../battle_insight/battle_insight.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_action
build/test_battle_action: $(SOURCES) $(HEADERS) test_battle_action.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_action.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_action
	./build/test_battle_action
clean:
	rm -f build/test_battle_action
