CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror

.PHONY: all test clean
all: build/test_wild_battle_setup

build/test_wild_battle_setup: wild_battle_setup.c wild_battle_setup.h test_wild_battle_setup.c ../call_wild_battle/call_wild_battle.c ../call_wild_battle/call_wild_battle.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) wild_battle_setup.c test_wild_battle_setup.c ../call_wild_battle/call_wild_battle.c $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_wild_battle_setup
	./build/test_wild_battle_setup

clean:
	rm -f build/test_wild_battle_setup
