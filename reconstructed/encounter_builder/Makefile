CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = encounter_builder.c test_encounter_builder.c test_encounter_chain.c ../battle_task/battle_task.c ../wild_battle_setup/wild_battle_setup.c ../call_wild_battle/call_wild_battle.c
HEADERS = encounter_builder.h ../battle_task/battle_task.h ../wild_battle_setup/wild_battle_setup.h ../call_wild_battle/call_wild_battle.h

.PHONY: all test clean
all: build/test_encounter_builder

build/test_encounter_builder: $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

test: build/test_encounter_builder
	./build/test_encounter_builder

clean:
	rm -f build/test_encounter_builder
