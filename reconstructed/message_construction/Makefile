CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = message_construction.c ../pokemon_resources/pokemon_resources.c ../archive_index/archive_index.c ../archive_io/archive_io.c ../message_resources/message_resources.c ../pokemon_move_resource/pokemon_move_resource.c
HEADERS = message_construction.h ../pokemon_resources/pokemon_resources.h ../archive_index/archive_index.h ../archive_io/archive_io.h ../message_resources/message_resources.h ../pokemon_move_resource/pokemon_move_resource.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_message_construction
build/test_message_construction: $(SOURCES) $(HEADERS) test_message_construction.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_message_construction.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_message_construction
	./build/test_message_construction
clean:
	rm -f build/test_message_construction
