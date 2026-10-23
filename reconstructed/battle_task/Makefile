CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror

.PHONY: all test clean
all: build/test_battle_task

build/test_battle_task: battle_task.c battle_task.h test_battle_task.c ../wild_battle_setup/wild_battle_setup.c ../wild_battle_setup/wild_battle_setup.h ../call_wild_battle/call_wild_battle.c ../call_wild_battle/call_wild_battle.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) battle_task.c test_battle_task.c ../wild_battle_setup/wild_battle_setup.c ../call_wild_battle/call_wild_battle.c $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_battle_task
	./build/test_battle_task

clean:
	rm -f build/test_battle_task
