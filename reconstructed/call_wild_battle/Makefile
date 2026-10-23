CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror

.PHONY: all test clean
all: build/test_call_wild_battle

build/test_call_wild_battle: call_wild_battle.c call_wild_battle.h test_call_wild_battle.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) call_wild_battle.c test_call_wild_battle.c $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_call_wild_battle
	./build/test_call_wild_battle

clean:
	rm -f build/test_call_wild_battle
