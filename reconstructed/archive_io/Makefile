CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
SOURCES = archive_io.c ../pokemon_growth/pokemon_growth.c ../pokemon_personal/pokemon_personal.c ../pokemon_stats/stats_math.c ../pokemon_data/pokemon_data.c ../pokemon_data/pokemon_codec.c ../pokemon_data/pokemon_fields.c ../pokemon_constructor/pokemon_constructor.c ../pokemon_constructor/pokemon_parameters.c
HEADERS = archive_io.h ../pokemon_growth/pokemon_growth.h ../pokemon_personal/pokemon_personal.h ../pokemon_stats/stats_math.h ../pokemon_stats/stats_fields.h ../pokemon_data/pokemon_data.h ../pokemon_constructor/pokemon_constructor.h

.PHONY: all test clean
all: build/test_archive_io
build/test_archive_io: $(SOURCES) $(HEADERS) test_archive_io.c
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) test_archive_io.c $(LDFLAGS) $(LDLIBS) -o $@
test: build/test_archive_io
	./build/test_archive_io
clean:
	rm -f build/test_archive_io
