CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = battle_core_5.c ../battle_core_4/battle_core_4.c ../battle_core_3/battle_core_3.c ../battle_core_2/battle_core_2.c ../battle_core_1/battle_core_1.c ../battle_tables_1/battle_tables_1.c ../battle_handler/battle_handler.c ../battle_items_1/battle_items_1.c ../battle_fields_1/battle_fields_1.c ../battle_modifier/battle_modifier.c ../battle_ability/battle_ability.c ../battle_abilities_1/battle_abilities_1.c ../battle_event/battle_event.c
HEADERS = battle_core_5.h ../battle_core_4/battle_core_4.h ../battle_core_3/battle_core_3.h ../battle_core_2/battle_core_2.h ../battle_core_1/battle_core_1.h ../battle_tables_1/battle_tables_1.h ../battle_handler/battle_handler.h ../battle_items_1/battle_items_1.h ../battle_fields_1/battle_fields_1.h ../battle_modifier/battle_modifier.h ../battle_abilities_1/battle_abilities_1.h ../battle_ability/battle_ability.h ../battle_event/battle_event.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_battle_core_5
build/test_battle_core_5: $(SOURCES) $(HEADERS) test_battle_core_5.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_battle_core_5.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_battle_core_5
	./build/test_battle_core_5
clean:
	rm -f build/test_battle_core_5
