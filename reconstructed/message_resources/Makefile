CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = message_resources.c ../pokemon_names/pokemon_names.c ../pokemon_data/pokemon_codec.c ../pokemon_constructor/pokemon_constructor.c ../pokemon_constructor/pokemon_parameters.c
HEADERS = message_resources.h ../pokemon_names/pokemon_names.h ../pokemon_data/pokemon_data.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_message_resources
build/test_message_resources: $(SOURCES) $(HEADERS) test_message_resources.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_message_resources.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_message_resources
	./build/test_message_resources
clean:
	rm -f build/test_message_resources
